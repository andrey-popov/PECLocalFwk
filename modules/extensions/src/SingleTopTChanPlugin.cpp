#include <PECFwk/extensions/SingleTopTChanPlugin.hpp>

#include <PECFwk/core/Processor.hpp>
#include <PECFwk/core/ROOTLock.hpp>

#include <TVector3.h>
#include <TMatrixDSym.h>
#include <TMatrixDSymEigen.h>

#include <sys/stat.h>


using namespace std;


SingleTopTChanPlugin::SingleTopTChanPlugin(string const &outDirectory_, BTagger const &bTagger_):
    AnalysisPlugin("SingleTop"),
    bTagger(bTagger_), outDirectory(outDirectory_)
{
    // Make sure the directory path ends with a slash
    if (outDirectory.back() != '/')
        outDirectory += '/';
    
    // Create the output directory if it does not exist
    struct stat dirStat;
    
    if (stat(outDirectory.c_str(), &dirStat) != 0)  // the directory does not exist
        mkdir(outDirectory.c_str(), 0755);
}


Plugin *SingleTopTChanPlugin::Clone() const
{
    return new SingleTopTChanPlugin(outDirectory, bTagger);
}


void SingleTopTChanPlugin::BeginRun(Dataset const &dataset)
{
    // Save pointer to the reader plugin
    reader = dynamic_cast<PECReaderPlugin const *>(master->GetPluginBefore("Reader", name));
    
    
    // Creation of ROOT objects is not thread-safe and must be protected
    ROOTLock::Lock();
    
    // Create the output file
    file = new TFile((outDirectory + dataset.GetFiles().front().GetBaseName() + ".root").c_str(),
     "recreate");
    
    // Create the tree
    tree = new TTree("Vars", "Basic kinematical variables");
    
    // End of critical block
    ROOTLock::Unlock();
    
    
    // Assign branch addresses
    tree->Branch("run", &runNumber);
    tree->Branch("event", &eventNumber);
    tree->Branch("lumiSection", &lumiSection);
    
    tree->Branch("Pt_Lep", &Pt_Lep);
    tree->Branch("Eta_Lep", &Eta_Lep);
    tree->Branch("MET", &MET);
    tree->Branch("MtW", &MtW);
    tree->Branch("Phi_MET", &Phi_MET);
    
    tree->Branch("Pt_J1", &Pt_J1);
    tree->Branch("Eta_J1", &Eta_J1);
    tree->Branch("Pt_J2", &Pt_J2);
    tree->Branch("Eta_J2", &Eta_J2);
    tree->Branch("Pt_LJ", &Pt_LJ);
    tree->Branch("Eta_LJ", &Eta_LJ);
    tree->Branch("Pt_BJ1", &Pt_BJ1);
    
    tree->Branch("M_J1J2", &M_J1J2);
    tree->Branch("DR_J1J2", &DR_J1J2);
    tree->Branch("Pt_J1J2", &Pt_J1J2);
    
    tree->Branch("Ht", &Ht);
    tree->Branch("M_JW", &M_JW);
    
    tree->Branch("Mtop_BJ1", &Mtop_BJ1);
    tree->Branch("Cos_LepLJ_BJ1", &Cos_LepLJ_BJ1);
    
    tree->Branch("Sphericity", &Sphericity);
    
    tree->Branch("nPV", &nPV);
    
    if (dataset.IsMC())
        tree->Branch("weight", &weight);
}


void SingleTopTChanPlugin::EndRun()
{
    // Operations with ROOT objects performed here are not thread-safe and must be guarded
    ROOTLock::Lock();
    
    // Write the tree and close the file
    file->cd();
    tree->Write("", TObject::kOverwrite);
    
    // Delete the objects
    delete tree;
    delete file;
    
    ROOTLock::Unlock();
}


bool SingleTopTChanPlugin::ProcessEvent()
{
    // Make sure the event contains reasonable physics objects
    if ((*reader)->GetLeptons().size() not_eq 1 or (*reader)->GetJets().size() < 2)
        return false;
    
    
    // Save event ID
    auto const &eventID = (*reader)->GetEventID();
    runNumber = eventID.Run();
    eventNumber = eventID.Event();
    lumiSection = eventID.LumiBlock();
    
    
    // Define some short-cuts
    auto const &lepton = (*reader)->GetLeptons().front();
    auto const &jets = (*reader)->GetJets();
    auto const &met = (*reader)->GetMET();
    
    
    // Calculate lepton-only variables
    Pt_Lep = lepton.Pt();
    Eta_Lep = lepton.Eta();
    MET = met.Pt();
    Phi_MET = met.Phi();
    
    MtW = sqrt(pow(lepton.Pt() + met.Pt(), 2) - pow(lepton.P4().Px() + met.P4().Px(), 2) -
     pow(lepton.P4().Py() + met.P4().Py(), 2));
    
    
    // Find the light-flavour jet and the hardest b-jet
    unsigned index = 0;
    Eta_LJ = 0.;
    
    for (unsigned i = 0; i < jets.size(); ++i)
        if (not bTagger(jets.at(i)) and fabs(jets.at(i).Eta()) > fabs(Eta_LJ))
        {
            index = i;
            Eta_LJ = jets.at(i).Eta();
        }
    
    auto const &lJet = jets.at(index);
    
    for (index = 0; index < jets.size(); ++index)
        if (bTagger(jets.at(index)))
            break;
    
    if (index == jets.size())  // there are no tagged jets
    {
        index = 0;
        double maxCSV = -100.;
        
        for (unsigned i = 0; i < jets.size(); ++i)
            if (jets.at(i).CSV() > maxCSV)
            {
                index = i;
                maxCSV = jets.at(i).CSV();
            }
    }
    
    auto const &bJet = jets.at(index);
    
    
    // Calculate single-jet variables
    Pt_J1 = jets.at(0).Pt();
    Eta_J1 = jets.at(0).Eta();
    Pt_J2 = jets.at(1).Pt();
    Eta_J2 = jets.at(1).Eta();
    Pt_BJ1 = bJet.Pt();
    Pt_LJ = lJet.Pt();
    
    
    // Calculate dijet variables
    M_J1J2 = (jets.at(0).P4() + jets.at(1).P4()).M();
    DR_J1J2 = jets.at(0).P4().DeltaR(jets.at(1).P4());
    Pt_J1J2 = (jets.at(0).P4() + jets.at(1).P4()).Pt();
    
    
    // Calculate multi-jet variables
    TLorentzVector p4Jets;
    Ht = 0.;
    
    for (auto const &j: jets)
    {
        p4Jets += j.P4();
        Ht += j.Pt();
    }
    
    for (auto const &j: (*reader)->GetAdditionalJets())
    {
        p4Jets += j.P4();
        Ht += j.Pt();
    }
    
    
    // Reconstruct W-boson
    TLorentzVector const p4W((*reader)->GetNeutrino().P4() + lepton.P4());
    
    M_JW = (p4W + p4Jets).M();
    
    
    // Reconstruct the top-quark
    TLorentzVector const p4Top(p4W + bJet.P4());
    
    Mtop_BJ1 = p4Top.M();
    
    
    // Calculate cos(theta)
    TVector3 b(p4Top.BoostVector());
    
    TLorentzVector boostedLepton(lepton.P4());
    boostedLepton.Boost(-b);
    TVector3 const p3Lepton(boostedLepton.Vect());
    
    TLorentzVector boostedLJet(lJet.P4());
    boostedLJet.Boost(-b);
    TVector3 const p3LJet(boostedLJet.Vect());
    
    Cos_LepLJ_BJ1 = p3Lepton.Dot(p3LJet) / (p3Lepton.Mag() * p3LJet.Mag());
    
    
    // Calculate sphericity
    TMatrixDSym sphericityTensor(3);
    double norm = 0.;
    
    TVector3 p3(p4W.Vect());
    norm += p3.Mag2();
    
    for (unsigned i = 0; i < 3; ++i)
        for (unsigned j = 0; j < 3; ++j)
            sphericityTensor(i, j) = p3[i] * p3[j];
    
    for (auto const &j: jets)
    {
        p3 = j.P4().Vect();
        norm += p3.Mag2();
        
        for (unsigned i = 0; i < 3; ++i)
            for (unsigned j = 0; j < 3; ++j)
                sphericityTensor(i, j) += p3[i] * p3[j];
    }
    
    sphericityTensor *= 1. / norm;
    
    TMatrixDSymEigen eigenValCalc(sphericityTensor);
    TVectorD eigenVals(eigenValCalc.GetEigenValues());
    
    Sphericity = 1.5 * (eigenVals[1] + eigenVals[2]);
    
    
    // Number of reconstructed primary vertices
    nPV = (*reader)->GetNPrimaryVertices();
    
    
    // Event weight
    weight = (*reader)->GetCentralWeight();
    
    
    tree->Fill();
    return true;
}
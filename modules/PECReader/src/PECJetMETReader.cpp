#include <mensura/PECReader/PECJetMETReader.hpp>

#include <mensura/core/Processor.hpp>
#include <mensura/core/PhysicsObjects.hpp>
#include <mensura/core/ROOTLock.hpp>
#include <mensura/PECReader/PECInputData.hpp>

#include <TVector2.h>

#include <cmath>
#include <limits>


PECJetMETReader::PECJetMETReader(std::string name /*= "JetMET"*/):
    JetMETReader(name),
    inputDataPluginName("InputData"),
    inputDataPlugin(nullptr),
    treeName("pecJetMET/JetMET"),
    bfJetPointer(&bfJets), bfMETPointer(&bfMETs),
    minPt(0.), maxAbsEta(std::numeric_limits<double>::infinity()),
    leptonPluginName("Leptons"), leptonPlugin(nullptr), leptonDR2(0.3 * 0.3),
    genJetPluginName(""), genJetPlugin(nullptr)
{}


PECJetMETReader::PECJetMETReader(PECJetMETReader const &src) noexcept:
    JetMETReader(src),
    inputDataPluginName(src.inputDataPluginName),
    inputDataPlugin(src.inputDataPlugin),
    treeName(src.treeName),
    bfJetPointer(&bfJets), bfMETPointer(&bfMETs),
    minPt(src.minPt), maxAbsEta(src.maxAbsEta),
    leptonPluginName(src.leptonPluginName), leptonPlugin(src.leptonPlugin),
    leptonDR2(src.leptonDR2),
    genJetPluginName(src.genJetPluginName), genJetPlugin(src.genJetPlugin)
{}


PECJetMETReader::~PECJetMETReader() noexcept
{}


void PECJetMETReader::ConfigureLeptonCleaning(std::string const leptonPluginName_ /*= "Leptons"*/,
  double dR /*= 0.3*/)
{
    leptonPluginName = leptonPluginName_;
    leptonDR2 = dR * dR;
}


void PECJetMETReader::BeginRun(Dataset const &)
{
    // Save pointer to the plugin providing access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(GetDependencyPlugin(inputDataPluginName));
    
    
    // Save pointers to plugins that produce leptons and generator-level jets
    if (leptonPluginName != "")
        leptonPlugin = dynamic_cast<LeptonReader const *>(GetDependencyPlugin(leptonPluginName));
    
    if (genJetPluginName != "")
        genJetPlugin = dynamic_cast<GenJetMETReader const *>(GetDependencyPlugin(genJetPluginName));
    
    
    // Set up the tree. Branches with properties that are not currently not used, are disabled
    inputDataPlugin->LoadTree(treeName);
    TTree *tree = inputDataPlugin->ExposeTree(treeName);
    ROOTLock::Lock();
    tree->SetBranchStatus("jets.jecUncertainty", false);
    tree->SetBranchStatus("jets.jerUncertainty", false);
    tree->SetBranchStatus("jets.bTagCMVA", false);
    tree->SetBranchStatus("jets.secVertexMass", false);
    tree->SetBranchStatus("jets.pileUpMVA", false);
    tree->SetBranchStatus("jets.area", false);
    tree->SetBranchStatus("jets.charge", false);
    tree->SetBranchStatus("jets.pullAngle", false);
    tree->SetBranchAddress("jets", &bfJetPointer);
    tree->SetBranchAddress("METs", &bfMETPointer);
    ROOTLock::Unlock();
}


Plugin *PECJetMETReader::Clone() const
{
    return new PECJetMETReader(*this);
}


double PECJetMETReader::GetJetRadius() const
{
    return 0.4;
}


void PECJetMETReader::SetGenJetReader(std::string const name /*= "GenJetMET"*/)
{
    genJetPluginName = name;
}


void PECJetMETReader::SetSelection(double minPt_, double maxAbsEta_)
{
    minPt = minPt_;
    maxAbsEta = maxAbsEta_;
}


bool PECJetMETReader::ProcessEvent()
{
    // Clear vector with jets from the previous event
    jets.clear();
    
    
    // Read jets and MET
    inputDataPlugin->ReadEventFromTree(treeName);
    
    // Collection of leptons against which jets will be cleaned
    auto const *leptonsForCleaning = (leptonPlugin) ? &leptonPlugin->GetLeptons() : nullptr;
    
    
    // Process jets in the current event
    for (pec::Jet const &j: bfJets)
    {
        // Read jet momentum and apply corrections to it
        TLorentzVector p4;
        p4.SetPtEtaPhiM(j.Pt(), j.Eta(), j.Phi(), j.M());
        p4 *= j.CorrFactor();
        
        
        // Loose physics selection
        if (not j.TestBit(1) /* "loose" jet ID */)
            continue;
        
        // User-defined selection on momentum
        if (p4.Pt() < minPt or fabs(p4.Eta()) > maxAbsEta)
            continue;
        
        
        // Peform cleaning against leptons if enabled
        if (leptonsForCleaning)
        {
            bool overlap = false;
            
            for (auto const &l: *leptonsForCleaning)
            {
                double const dR2 = std::pow(p4.Eta() - l.Eta(), 2) +
                  std::pow(TVector2::Phi_mpi_pi(p4.Phi() - l.Phi()), 2);
                //^ Do not use TLorentzVector::DeltaR to avoid calculating sqrt
                
                if (dR2 < leptonDR2)
                {
                    overlap = true;
                    break;
                }
            }
            
            if (overlap)
                continue;
        }
        
        
        // Build the jet object. At this point jet momentum must be fully corrected
        Jet jet(p4);
        
        jet.SetBTag(BTagger::Algorithm::CSV, j.BTagCSV());
        // jet.SetArea(j.Area());
        // jet.SetCharge(j.Charge());
        // jet.SetPullAngle(j.PullAngle());
        
        jet.SetParentID(j.Flavour());
        
        
        // Perform matching to generator-level jets if the corresponding reader is available. Choose
        //the closest jet but require that the angular separation is not larger than half of the
        //radius parameter of the reconstructed jets
        if (genJetPlugin)
        {
            double minDR2 = std::pow(GetJetRadius() / 2., 2);
            GenJet const *matchedGenJet = nullptr;
            
            for (auto const &genJet: genJetPlugin->GetJets())
            {
                double const dR2 = std::pow(p4.Eta() - genJet.Eta(), 2) +
                 std::pow(TVector2::Phi_mpi_pi(p4.Phi() - genJet.Phi()), 2);
                //^ Do not use TLorentzVector::DeltaR to avoid calculating sqrt
                
                if (dR2 < minDR2)
                {
                    matchedGenJet = &genJet;
                    minDR2 = dR2;
                }
            }
            
            jet.SetMatchedGenJet(matchedGenJet);
        }
        
        
        jets.push_back(jet);
    }
    
    
    // Make sure collection of jets is ordered in transverse momentum
    std::sort(jets.rbegin(), jets.rend());
    
    
    // Copy corrected MET
    pec::Candidate const &correctedMET = bfMETs.at(0);
    met.SetPtEtaPhiM(correctedMET.Pt(), 0., correctedMET.Phi(), 0.);
    
    
    
    // Since this reader does not have access to the input file, it does not know when there are
    //no more events in the dataset and thus always returns true
    return true;
}


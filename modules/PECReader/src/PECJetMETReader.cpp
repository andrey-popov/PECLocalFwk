#include <mensura/PECReader/PECJetMETReader.hpp>

#include <mensura/core/Processor.hpp>
#include <mensura/core/PhysicsObjects.hpp>
#include <mensura/core/ROOTLock.hpp>
#include <mensura/PECReader/PECInputData.hpp>

#include <TVector2.h>

#include <cmath>
#include <iostream>
#include <limits>


PECJetMETReader::PECJetMETReader(std::string name /*= "JetMET"*/):
    JetMETReader(name),
    inputDataPluginName("InputData"), inputDataPlugin(nullptr),
    systServiceName("Systematics"),
    treeName("pecJetMET/JetMET"),
    bfJetPointer(&bfJets), bfMETPointer(&bfMETs),
    minPt(0.), maxAbsEta(std::numeric_limits<double>::infinity()),
    leptonPluginName("Leptons"), leptonPlugin(nullptr),
    genJetPluginName(""), genJetPlugin(nullptr),
    systType(SystType::None), systDirection(0)
{
    leptonDR2 = std::pow(GetJetRadius(), 2);
}


PECJetMETReader::PECJetMETReader(PECJetMETReader const &src) noexcept:
    JetMETReader(src),
    inputDataPluginName(src.inputDataPluginName), inputDataPlugin(src.inputDataPlugin),
    systServiceName(src.systServiceName),
    treeName(src.treeName),
    bfJetPointer(&bfJets), bfMETPointer(&bfMETs),
    minPt(src.minPt), maxAbsEta(src.maxAbsEta),
    leptonPluginName(src.leptonPluginName), leptonPlugin(src.leptonPlugin),
    leptonDR2(src.leptonDR2),
    genJetPluginName(src.genJetPluginName), genJetPlugin(src.genJetPlugin),
    systType(src.systType), systDirection(src.systDirection)
{}


void PECJetMETReader::BeginRun(Dataset const &)
{
    // Save pointer to the plugin providing access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(GetDependencyPlugin(inputDataPluginName));
    
    // Save pointers to plugins that produce leptons and generator-level jets
    if (leptonPluginName != "")
        leptonPlugin = dynamic_cast<LeptonReader const *>(GetDependencyPlugin(leptonPluginName));
    
    if (genJetPluginName != "")
        genJetPlugin = dynamic_cast<GenJetMETReader const *>(GetDependencyPlugin(genJetPluginName));
    
    
    // Read requested systematic variation
    if (systServiceName != "")
    {
        SystService const *systService =
          dynamic_cast<SystService const *>(GetMaster().GetServiceQuiet(systServiceName));
        
        if (systService)
        {
            std::pair<bool, SystService::VarDirection> s;
            
            if ((s = systService->Test("JEC")).first)
            {
                systType = SystType::JEC;
                systDirection = (s.second == SystService::VarDirection::Up) ? +1 : -1;
            }
            else if ((s = systService->Test("JER")).first)
            {
                systType = SystType::JER;
                systDirection = (s.second == SystService::VarDirection::Up) ? +1 : -1;
            }
            else if ((s = systService->Test("METUncl")).first)
            {
                systType = SystType::METUncl;
                systDirection = (s.second == SystService::VarDirection::Up) ? +1 : -1;
            }
        }
    }
    
    
    // Set up the tree. Branches with properties that are not currently not used, are disabled
    inputDataPlugin->LoadTree(treeName);
    TTree *tree = inputDataPlugin->ExposeTree(treeName);
    
    ROOTLock::Lock();
    
    if (systType != SystType::JEC)
        tree->SetBranchStatus("jets.jecUncertainty", false);
    
    if (systType != SystType::JER)
        tree->SetBranchStatus("jets.jerUncertainty", false);
    
    tree->SetBranchStatus("jets.bTagCMVA", false);
    tree->SetBranchStatus("jets.secVertexMass", false);
    // tree->SetBranchStatus("jets.area", false);
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


void PECJetMETReader::ConfigureLeptonCleaning(std::string const leptonPluginName_, double dR)
{
    leptonPluginName = leptonPluginName_;
    leptonDR2 = dR * dR;
}


void PECJetMETReader::ConfigureLeptonCleaning(std::string const leptonPluginName_ /*= "Leptons"*/)
{
    leptonPluginName = leptonPluginName_;
    leptonDR2 = std::pow(GetJetRadius(), 2);
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
    
    
    // Header for debug print out
    #ifdef DEBUG
    std::cout << "PECJetMETReader[\"" << GetName() << "\"]: Jets in the current event:\n";
    unsigned curJetNumber = 0;
    #endif
    
    
    // Process jets in the current event
    for (pec::Jet const &j: bfJets)
    {
        // Read raw jet momentum and apply corrections to it. The correction factor read from
        //pec::Jet is zero if only raw momentum is stored. In this case propagate the raw momentum
        //unchanged.
        TLorentzVector p4;
        p4.SetPtEtaPhiM(j.Pt(), j.Eta(), j.Phi(), j.M());
        
        double const corrFactor = j.CorrFactor();
        
        if (corrFactor != 0.)
            p4 *= corrFactor;
        
        
        #ifdef DEBUG
        ++curJetNumber;
        std::cout << " Jet #" << curJetNumber << "\n";
        std::cout << "  Raw momentum (pt, eta, phi, m): " << j.Pt() << ", " << j.Eta() << ", " <<
          j.Phi() << ", " << j.M() << '\n';
        std::cout << "  Fully corrected pt: " << p4.Pt() << '\n';
        std::cout << "  JEC uncertainty: " << j.JECUncertainty() << ", JER uncertainty: " <<
          j.JERUncertainty() << '\n';
        #endif
        
        
        // Apply systematic variations if requested
        if (systType == SystType::JEC)
            p4 *= 1. + systDirection * j.JECUncertainty();
        else if (systType == SystType::JER)
            p4 *= 1. + systDirection * j.JERUncertainty();
        
        
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
        
        
        #ifdef DEBUG
        std::cout << "  Jet passes selection on kinematics and ID and does not overlap with " <<
          "a lepton\n";
        #endif
        
        
        // Build the jet object. At this point jet momentum must be fully corrected
        Jet jet;
        
        if (corrFactor != 0.)
            jet.SetCorrectedP4(p4, 1. / corrFactor);
        else
            jet.SetCorrectedP4(p4, 1.);
        
        jet.SetBTag(BTagger::Algorithm::CSV, j.BTagCSV());
        jet.SetArea(j.Area());
        // jet.SetCharge(j.Charge());
        // jet.SetPullAngle(j.PullAngle());
        jet.SetPileUpID(j.PileUpID());
        
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
        
        #ifdef DEBUG
        std::cout << "  Flavour: " << j.Flavour() << ", CSV value: " << j.BTagCSV() << '\n';
        std::cout << "  Has a GEN-level match? ";
        
        if (genJetPlugin)
        {
            if (jet.MatchedGenJet())
                std::cout << "yes";
            else
                std::cout << "no";
        }
        else
            std::cout << "n/a";
        
        std::cout << '\n';
        #endif
        
        
        jets.push_back(jet);
    }
    
    
    // Make sure collection of jets is ordered in transverse momentum
    std::sort(jets.rbegin(), jets.rend());
    
    
    // Copy corrected MET corresponding to the requested systematic variation
    unsigned metIndex;
    
    switch (systType)
    {
        case SystType::JEC:
            metIndex = (systDirection > 0) ? 2 : 3;
            break;
        
        case SystType::JER:
            metIndex = (systDirection > 0) ? 4 : 5;
            break;
        
        case SystType::METUncl:
            metIndex = (systDirection > 0) ? 6 : 7;
            break;
        
        default:
            metIndex = 0;
    }
    
    pec::Candidate const &correctedMET = bfMETs.at(metIndex);
    met.SetPtEtaPhiM(correctedMET.Pt(), 0., correctedMET.Phi(), 0.);
    
    
    #ifdef DEBUG
    std::cout << "PECJetMETReader[\"" << GetName() << "\"]: MET in the current event:\n";
    std::cout << " Raw MET (pt, phi): " << bfMETs.at(1).Pt() << ", " << bfMETs.at(1).Phi() << '\n';
    std::cout << " Corrected MET (pt, phi): " << bfMETs.at(0).Pt() << ", "
      << bfMETs.at(0).Phi() << std::endl;
    #endif
    
    
    
    // Since this reader does not have access to the input file, it does not know when there are
    //no more events in the dataset and thus always returns true
    return true;
}


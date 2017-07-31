#include <mensura/PECReader/PECJetMETReader.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/core/PileUpReader.hpp>
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
    bfJetPointer(&bfJets), bfMETPointer(&bfMETs), bfUncorrMETPointer(&bfUncorrMETs),
    minPt(0.), maxAbsEta(std::numeric_limits<double>::infinity()),
    readRawMET(false), applyJetID(true),
    leptonPluginName("Leptons"), leptonPlugin(nullptr),
    genJetPluginName(""), genJetPlugin(nullptr),
    puPluginName("PileUp"), puPlugin(nullptr),
    jerFilePath(""), jerPtFactor(0.),
    systType(SystType::None), systDirection(0)
{
    leptonDR2 = std::pow(GetJetRadius(), 2);
}


PECJetMETReader::PECJetMETReader(PECJetMETReader const &src) noexcept:
    JetMETReader(src),
    inputDataPluginName(src.inputDataPluginName), inputDataPlugin(src.inputDataPlugin),
    systServiceName(src.systServiceName),
    treeName(src.treeName),
    bfJetPointer(&bfJets), bfMETPointer(&bfMETs), bfUncorrMETPointer(&bfUncorrMETs),
    minPt(src.minPt), maxAbsEta(src.maxAbsEta),
    readRawMET(src.readRawMET), applyJetID(src.applyJetID),
    leptonPluginName(src.leptonPluginName), leptonPlugin(src.leptonPlugin),
    leptonDR2(src.leptonDR2),
    genJetPluginName(src.genJetPluginName), genJetPlugin(src.genJetPlugin),
    puPluginName(src.puPluginName), puPlugin(src.puPlugin),
    jerFilePath(src.jerFilePath), jerPtFactor(src.jerPtFactor),
    systType(src.systType), systDirection(src.systDirection)
{}


void PECJetMETReader::BeginRun(Dataset const &)
{
    // Save pointers to required plugins
    inputDataPlugin = dynamic_cast<PECInputData const *>(GetDependencyPlugin(inputDataPluginName));
    
    if (leptonPluginName != "")
        leptonPlugin = dynamic_cast<LeptonReader const *>(GetDependencyPlugin(leptonPluginName));
    
    if (genJetPluginName != "")
        genJetPlugin = dynamic_cast<GenJetMETReader const *>(GetDependencyPlugin(genJetPluginName));
    
    if (jerFilePath != "")
        puPlugin = dynamic_cast<PileUpReader const *>(GetDependencyPlugin(puPluginName));
    
    
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
    
    tree->SetBranchStatus("jets.charge", false);
    tree->SetBranchStatus("jets.pullAngle", false);
    
    tree->SetBranchAddress("jets", &bfJetPointer);
    tree->SetBranchAddress("METs", &bfMETPointer);
    
    if (readRawMET)
        tree->SetBranchAddress("uncorrMETs", &bfUncorrMETPointer);
    
    ROOTLock::Unlock();
    
    
    // Create an object to access jet pt resolution
    if (jerFilePath != "")
        jerProvider.reset(new JME::JetResolution(jerFilePath));
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


void PECJetMETReader::ReadRawMET(bool enable /*= true*/)
{
    readRawMET = enable;
}


void PECJetMETReader::SetApplyJetID(bool applyJetID_)
{
    applyJetID = applyJetID_;
}


void PECJetMETReader::SetGenJetReader(std::string const name /*= "GenJetMET"*/)
{
    genJetPluginName = name;
}


void PECJetMETReader::SetGenPtMatching(std::string const &jerFile, double jerPtFactor_ /*= 3.*/)
{
    jerFilePath = FileInPath::Resolve("JERC", jerFile);
    jerPtFactor = jerPtFactor_;
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
        
        double corrFactor = j.CorrFactor();
        
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
        double systCorrFactor = 1.;
        
        if (systType == SystType::JEC)
            systCorrFactor = 1. + systDirection * j.JECUncertainty();
        else if (systType == SystType::JER)
            systCorrFactor = 1. + systDirection * j.JERUncertainty();
        
        p4 *= systCorrFactor;
        corrFactor *= systCorrFactor;
        
        
        // Loose physics selection
        if (applyJetID and not j.TestBit(1) /* "loose" jet ID */)
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
        
        jet.SetBTag(BTagger::Algorithm::CSV, j.BTag(pec::Jet::BTagAlgo::CSV));
        jet.SetBTag(BTagger::Algorithm::CMVA, j.BTag(pec::Jet::BTagAlgo::CMVA));
        jet.SetBTag(BTagger::Algorithm::DeepCSV,
          j.BTagDNN(pec::Jet::BTagDNNType::BB) + j.BTagDNN(pec::Jet::BTagDNNType::B));
        
        jet.SetArea(j.Area());
        // jet.SetCharge(j.Charge());
        // jet.SetPullAngle(j.PullAngle());
        jet.SetPileUpID(j.PileUpID());
        
        jet.SetFlavour(Jet::FlavourType::Hadron, j.Flavour(pec::Jet::FlavourType::Hadron));
        jet.SetFlavour(Jet::FlavourType::Parton, j.Flavour(pec::Jet::FlavourType::Parton));
        jet.SetFlavour(Jet::FlavourType::ME, j.Flavour(pec::Jet::FlavourType::ME));
        
        if (not applyJetID)
            jet.SetUserInt("ID", int(j.TestBit(1)));
        
        
        // Perform matching to generator-level jets if the corresponding reader is available.
        //Choose the closest jet but require that the angular separation is not larger than half of
        //the radius parameter of reconstructed jets and, if the plugin has been configured to
        //check this, that the difference in pt is compatible with the pt resolution in simulation.
        if (genJetPlugin)
        {
            double minDR2 = std::pow(GetJetRadius() / 2., 2);
            GenJet const *matchedGenJet = nullptr;
            double maxDPt = std::numeric_limits<double>::infinity();
            
            if (jerProvider)
            {
                double const ptResolution = jerProvider->getResolution(
                  {{JME::Binning::JetPt, p4.Pt()}, {JME::Binning::JetEta, p4.Eta()},
                  {JME::Binning::Rho, puPlugin->GetRho()}});
                maxDPt = ptResolution * p4.Pt() * jerPtFactor;
            }
            
            
            for (auto const &genJet: genJetPlugin->GetJets())
            {
                double const dR2 = std::pow(p4.Eta() - genJet.Eta(), 2) +
                  std::pow(TVector2::Phi_mpi_pi(p4.Phi() - genJet.Phi()), 2);
                //^ Do not use TLorentzVector::DeltaR to avoid calculating sqrt
                
                if (dR2 < minDR2 and std::abs(p4.Pt() - genJet.Pt()) < maxDPt)
                {
                    matchedGenJet = &genJet;
                    minDR2 = dR2;
                }
            }
            
            jet.SetMatchedGenJet(matchedGenJet);
        }
        
        #ifdef DEBUG
        std::cout << "  Flavour: " << j.Flavour() << ", CSV value: " <<
          j.BTag(pec::Jet::BTagAlgo::CSV) << '\n';
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
            metIndex = (systDirection > 0) ? 1 : 2;
            break;
        
        case SystType::JER:
            metIndex = (systDirection > 0) ? 3 : 4;
            break;
        
        case SystType::METUncl:
            metIndex = (systDirection > 0) ? 5 : 6;
            break;
        
        default:
            metIndex = 0;
    }
    
    pec::Candidate const &correctedMET = bfMETs.at(metIndex);
    met.SetPtEtaPhiM(correctedMET.Pt(), 0., correctedMET.Phi(), 0.);
    
    
    // Copy raw MET if requested
    if (readRawMET)
    {
        pec::Candidate const &pecRawMET = bfUncorrMETs.at(0);
        rawMET.SetPtEtaPhiM(pecRawMET.Pt(), 0., pecRawMET.Phi(), 0.);
    }
    
    
    #ifdef DEBUG
    std::cout << "PECJetMETReader[\"" << GetName() << "\"]: MET in the current event:\n";
    std::cout << " Raw MET (pt, phi): " << rawMET.Pt() << ", " << rawMET.Phi() << '\n';
    std::cout << " Corrected MET (pt, phi): " << bfMETs.at(0).Pt() << ", "
      << bfMETs.at(0).Phi() << std::endl;
    #endif
    
    
    
    // Since this reader does not have access to the input file, it does not know when there are
    //no more events in the dataset and thus always returns true
    return true;
}


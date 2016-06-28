#include <mensura/extensions/JetMETUpdate.hpp>

#include <mensura/core/PileUpReader.hpp>
#include <mensura/core/Processor.hpp>

#include <cmath>
#include <limits>
#include <sstream>
#include <stdexcept>


JetMETUpdate::JetMETUpdate(std::string const name /*= "JetMET"*/):
    JetMETReader(name),
    jetmetPlugin(nullptr), jetmetPluginName("OrigJetMET"),
    puPlugin(nullptr), puPluginName("PileUp"),
    systServiceName("Systematics"),
    jetCorrForJets(nullptr),
    jetCorrForMETFull(nullptr), jetCorrForMETL1(nullptr),
    jetCorrForMETOrigFull(nullptr), jetCorrForMETOrigL1(nullptr),
    minPt(0.), maxAbsEta(std::numeric_limits<double>::infinity()), minPtForT1(15.),
    useRawMET(false)
{}


void JetMETUpdate::BeginRun(Dataset const &)
{
    // Save pointers to the original JetMETReader and a PileUpReader
    jetmetPlugin = dynamic_cast<JetMETReader const *>(GetDependencyPlugin(jetmetPluginName));
    puPlugin = dynamic_cast<PileUpReader const *>(GetDependencyPlugin(puPluginName));
    
    
    // Read requested systematic variation
    systType = JetCorrectorService::SystType::None;
    systDirection = SystService::VarDirection::Undefined;
    
    if (systServiceName != "")
    {
        SystService const *systService =
          dynamic_cast<SystService const *>(GetMaster().GetServiceQuiet(systServiceName));
        
        if (systService)
        {
            std::pair<bool, SystService::VarDirection> s;
            
            if ((s = systService->Test("JEC")).first)
            {
                systType = JetCorrectorService::SystType::JEC;
                systDirection = s.second;
            }
            else if ((s = systService->Test("JER")).first)
            {
                systType = JetCorrectorService::SystType::JER;
                systDirection = s.second;
            }
        }
    }
    
    
    // Read services for jet corrections
    for (auto const &p: {make_pair(&jetCorrForJetsName, &jetCorrForJets),
      make_pair(&jetCorrForMETFullName, &jetCorrForMETFull),
      make_pair(&jetCorrForMETL1Name, &jetCorrForMETL1),
      make_pair(&jetCorrForMETOrigFullName, &jetCorrForMETOrigFull),
      make_pair(&jetCorrForMETOrigL1Name, &jetCorrForMETOrigL1)})
    {
        if (*p.first != "")
            *p.second =
              dynamic_cast<JetCorrectorService const *>(GetMaster().GetService(*p.first));
    }
}


Plugin *JetMETUpdate::Clone() const
{
    return new JetMETUpdate(*this);
}


double JetMETUpdate::GetJetRadius() const
{
    if (not jetmetPlugin)
    {
        std::ostringstream message;
        message << "JetMETUpdate[\"" << GetName() << "\"]::GetJetRadius: This method cannot be " <<
          "executed before a handle to the original JetMETReader has been obtained.";
        throw std::runtime_error(message.str());
    }
    
    return jetmetPlugin->GetJetRadius();
}


void JetMETUpdate::SetJetCorrection(std::string const &jetCorrServiceName)
{
    jetCorrForJetsName = jetCorrServiceName;
}


void JetMETUpdate::SetJetCorrectionForMET(std::string const &fullNew, std::string const &l1New,
  std::string const &fullOrig, std::string const &l1Orig)
{
    // If new and original correctors are the same, drop them since their effect would cancel out
    if (fullNew != fullOrig)
    {
        jetCorrForMETFullName = fullNew;
        jetCorrForMETOrigFullName = fullOrig;
    }
    else
        jetCorrForMETFullName = jetCorrForMETOrigFullName = "";
    
    if (l1New != l1Orig)
    {
        jetCorrForMETL1Name = l1New;
        jetCorrForMETOrigL1Name = l1Orig;
    }
    else
        jetCorrForMETL1Name = jetCorrForMETOrigL1Name = "";
}


void JetMETUpdate::SetSelection(double minPt_, double maxAbsEta_)
{
    minPt = minPt_;
    maxAbsEta = maxAbsEta_;
}


void JetMETUpdate::UseRawMET(bool set /*= true*/)
{
    useRawMET = set;
}


bool JetMETUpdate::ProcessEvent()
{
    jets.clear();
    
    
    // Read value of rho
    double const rho = puPlugin->GetRho();
    
    
    // A shift to be applied to MET to account for differences in T1 corrections
    TLorentzVector metShift;
    
    
    // Loop over original collection of jets
    for (auto const &srcJet: jetmetPlugin->GetJets())
    {
        // Copy current jet and recorrect its momentum
        Jet jet(srcJet);
        
        if (jetCorrForJets)
        {
            double const corrFactor = jetCorrForJets->Eval(srcJet, rho, systType, systDirection);
            jet.SetCorrectedP4(srcJet.RawP4() * corrFactor, 1. / corrFactor);
        }
        
        
        // Compute the shift in MET due to T1 corrections. Systematic variations for L1 corrections
        //are ignored.
        if (jet.Pt() > minPtForT1)
        {
            // Undo applied T1 corrections
            if (jetCorrForMETOrigL1)
                metShift -=
                  srcJet.RawP4() * jetCorrForMETOrigL1->Eval(srcJet, rho);
            
            if (jetCorrForMETOrigFull)
                metShift +=
                  srcJet.RawP4() * jetCorrForMETOrigFull->Eval(srcJet, rho, systType, systDirection);
            
            
            // Apply new T1 corrections
            if (jetCorrForMETL1)
                metShift +=
                  srcJet.RawP4() * jetCorrForMETL1->Eval(srcJet, rho);
            
            if (jetCorrForMETFull)
                metShift -=
                  srcJet.RawP4() * jetCorrForMETFull->Eval(srcJet, rho, systType, systDirection);
        }
        
        
        // Store the new jet if it passes the kinematical selection
        if (jet.Pt() > minPt and std::abs(jet.Eta()) < maxAbsEta)
            jets.emplace_back(jet);
    }
    
    
    // Make sure the new collection of jets is ordered in transverse momentum
    std::sort(jets.rbegin(), jets.rend());
    
    
    // Update MET
    TLorentzVector const &startingMET = (useRawMET) ?
      jetmetPlugin->GetRawMET().P4() : jetmetPlugin->GetMET().P4();
    TLorentzVector updatedMET(startingMET + metShift);
    met.SetPtEtaPhiM(updatedMET.Pt(), 0., updatedMET.Phi(), 0.);
    
    
    return true;
}

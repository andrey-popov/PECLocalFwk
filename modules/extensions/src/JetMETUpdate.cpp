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
    minPt(0.), maxAbsEta(std::numeric_limits<double>::infinity())
{}


void JetMETUpdate::BeginRun(Dataset const &)
{
    // Save pointers to the original JetMETReader and a PileUpReader
    jetmetPlugin = dynamic_cast<JetMETReader const *>(GetDependencyPlugin(jetmetPluginName));
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


void JetMETUpdate::SetJetCorrection(std::string jetCorrServiceName)
{
    jetCorrForJetsName = jetCorrServiceName;
}


void JetMETUpdate::SetSelection(double minPt_, double maxAbsEta_)
{
    minPt = minPt_;
    maxAbsEta = maxAbsEta_;
}


bool JetMETUpdate::ProcessEvent()
{
    jets.clear();
    
    
    // Read value of rho
    double const rho = puPlugin->GetRho();
    
    
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
        
        
        // Store the new jet if it passes the kinematical selection
        if (jet.Pt() > minPt and std::abs(jet.Eta()) < maxAbsEta)
            jets.emplace_back(jet);
    }
    
    
    // Make sure the new collection of jets is ordered in transverse momentum
    std::sort(jets.rbegin(), jets.rend());
    
    
    return true;
}
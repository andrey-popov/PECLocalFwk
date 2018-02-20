#include <mensura/extensions/JetCorrectorService.hpp>

#include <mensura/core/PhysicsObjects.hpp>
#include <mensura/core/ROOTLock.hpp>

#include <mensura/external/JERC/JetCorrectorParameters.hpp>

#include <cmath>
#include <iostream>
#include <sstream>
#include <stdexcept>


JetCorrectorService::IOVParams::IOVParams(unsigned long minRun_, unsigned long maxRun_):
    minRun(minRun_), maxRun(maxRun_)
{}


JetCorrectorService::JetCorrectorService(std::string const name /*= "JetCorrector"*/):
    Service(name),
    matchAllMode(false), curIOV(-1), curRun(0)
{}


JetCorrectorService::JetCorrectorService(JetCorrectorService const &src):
    Service(src),
    iovParams(src.iovParams), iovLabelMap(src.iovLabelMap),
    matchAllMode(src.matchAllMode), curIOV(-1), curRun(0)
{
    // Create a random-number generator if needed. Cannot share the same generator between copies
    //because generation of random numbers is not thread-safe
    if (src.rGen)
    {
        ROOTLock::Lock();
        rGen.reset(new TRandom3(src.rGen->GetSeed()));
        ROOTLock::Unlock();
    }
}


Service *JetCorrectorService::Clone() const
{
    return new JetCorrectorService(*this);
}


double JetCorrectorService::Eval(Jet const &jet, double rho, SystType syst /*= SystType::None*/,
  SystService::VarDirection direction /*= SystService::VarDirection::Undefined*/) const
{
    if (iovParams.empty())
    {
        std::ostringstream message;
        message << "JetCorrectorService[\"" << GetName() << "\"]::Eval: Service has not been "
          "configured.";
        throw std::logic_error(message.str());
    }
    
    
    // Variable that will accumulate the total correction factor
    double corrFactor = 1.;
    
    
    // Evaluate nominal jet energy correction. Save corresponding jet pt.
    double jecCorrPt;
    
    if (jetEnergyCorrector)
    {
        double const rawPt = jet.RawP4().Pt();
        
        jetEnergyCorrector->setJetEta(jet.Eta());
        jetEnergyCorrector->setJetPt(rawPt);
        jetEnergyCorrector->setJetA(jet.Area());
        jetEnergyCorrector->setRho(rho);
        
        double const jecFactor = jetEnergyCorrector->getCorrection();
        
        corrFactor *= jecFactor;
        jecCorrPt = rawPt * jecFactor;
    }
    else
    {
        // Assume the jet has been created with momentum fully corrected for nominal JEC
        jecCorrPt = jet.Pt();
    }
    
    
    // Evaluate systematical variation for JEC
    if (syst == SystType::JEC)
    {
        // Sanity check
        if (jecUncProviders.size() == 0)
        {
            std::ostringstream message;
            message << "JetCorrectorService[\"" << GetName() << "\"]::Eval: Cannot evaluate JEC "
              "systematics because no uncertainties have been specified.";
            throw std::logic_error(message.str());
        }
        
        
        double const jecUncertainty = EvalJECUnc(jecCorrPt, jet.Eta());
        
        if (direction == SystService::VarDirection::Up)
            corrFactor *= (1. + jecUncertainty);
        else if (direction == SystService::VarDirection::Down)
            corrFactor *= (1. - jecUncertainty);
    }
    
    
    // Sanity check before JER smearing
    if (syst == SystType::JER and direction != SystService::VarDirection::Undefined and
      not jerSFProvider)
    {
        std::ostringstream message;
        message << "JetCorrectorService[\"" << GetName() << "\"]::Eval: Cannot evaluate JER "
          "systematics because JER scale factors have not been specified.";
        throw std::logic_error(message.str());
    }
    
    
    // Evaluate JER smearing. Corresponding correction factor is always evaluated after nominal
    //JEC are applied, even when a systematic variaion in JEC is requested. This done to be aligned
    //with the way JER smearing is usually applied in CMSSW.
    if (jerSFProvider)
    {
        // Find data/MC scale factor for pt resolution for the current jet
        Variation jerVar = Variation::NOMINAL;
        
        if (syst == SystType::JER)
        {
            if (direction == SystService::VarDirection::Up)
                jerVar = Variation::UP;
            else if (direction == SystService::VarDirection::Down)
                jerVar = Variation::DOWN;
        }
        
        double const jerSF =
          jerSFProvider->getScaleFactor({{JME::Binning::JetEta, jet.Eta()}}, jerVar);
        
        
        // Depending on the presence of a matched GEN-level jet, perform deterministic or
        //stochastic smearing
        GenJet const *genJet = jet.MatchedGenJet();
        
        if (genJet)
        {
            // Smearing is done as here [1]
            //[1] https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_8/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h#L236-L237
            double const jerFactor = 1. + (jerSF - 1.) * (jecCorrPt - genJet->Pt()) / jecCorrPt;
            
            // Different definition for debugging with PEC tuples 3.1.0
            // double const jecCorrE = jet.RawP4().E() * jecCorrPt / jet.RawP4().Pt();
            // double const jerFactor = 1. + (jerSF - 1.) * (jecCorrE - genJet->E()) / jecCorrE;
            
            corrFactor *= jerFactor;
        }
        else if (jerProvider)
        {
            // Follow the same approach as here [1]
            //[1] https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_8/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h#L244_L250
            double const ptResolution =
              jerProvider->getResolution({{JME::Binning::JetPt, jecCorrPt},
              {JME::Binning::JetEta, jet.Eta()}, {JME::Binning::Rho, rho}});
            double const jerFactor = 1. + rGen->Gaus(0., ptResolution) *
              std::sqrt(std::max(std::pow(jerSF, 2) - 1., 0.));
            
            corrFactor *= jerFactor;
        }
    }
    
    
    return corrFactor;
}


double JetCorrectorService::EvalJECUnc(double const corrPt, double const eta) const
{
    if (iovParams.empty())
    {
        std::ostringstream message;
        message << "JetCorrectorService[\"" << GetName() << "\"]::EvalJECUnc: Service has not "
          "been configured.";
        throw std::logic_error(message.str());
    }
    
    
    double unc;
    
    try
    {
        if (jecUncProviders.size() == 1)
        {
            // Consider the case of a single uncertainty specially in order to avoid unnecessary
            //computation of sqrt(unc^2)
            auto &acc = jecUncProviders.front();
            acc->setJetEta(eta);
            acc->setJetPt(corrPt);
            
            unc = acc->getUncertainty(true);
        }
        else
        {
            double unc2 = 0.;
            
            for (auto &acc: jecUncProviders)
            {
                acc->setJetEta(eta);
                acc->setJetPt(corrPt);
                
                unc2 += std::pow(acc->getUncertainty(true), 2);
            }
            
            unc = std::sqrt(unc2);
        }
    }
    catch (std::out_of_range const &e)
    {
        std::ostringstream message;
        message << "Failed to evaluate JEC uncertainty for jet with corrected pt = " << corrPt <<
          ", eta = " << eta << "\n";
        throw std::runtime_error(message.str());
    };
    
    return unc;
}


bool JetCorrectorService::IsSystEnabled(SystType syst) const
{
    switch (syst)
    {
        case SystType::JEC:
            return not jecUncProviders.empty();
        
        case SystType::JER:
            return bool(jerSFProvider);
        
        case SystType::None:
            return true;
        
        default:
            return false;
    }
}


double JetCorrectorService::operator()(Jet const &jet, double rho, SystType syst /*= SystType::None*/,
  SystService::VarDirection direction /*= SystService::VarDirection::Undefined*/) const
{
    return Eval(jet, rho, syst, direction);
}


void JetCorrectorService::RegisterIOV(std::string const &label, unsigned long minRun,
  unsigned long maxRun)
{
    // Sanity checks
    if (label == "")
    {
        std::ostringstream message;
        message << "JetCorrectorService[\"" << GetName() << "\"]::RegisterIOV: An empty label is "
          "not allowed.";
        throw std::runtime_error(message.str());
    }
    
    if (iovLabelMap.find(label) != iovLabelMap.end())
    {
        std::ostringstream message;
        message << "JetCorrectorService[\"" << GetName() << "\"]::RegisterIOV: An IOV with "
          "label \"" << label << "\" has already been registered.";
        throw std::runtime_error(message.str());
    }
    
    for (auto const &iov: iovParams)
        if ((minRun >= iov.minRun and minRun <= iov.maxRun) or
          (maxRun >= iov.minRun and maxRun <= iov.maxRun))
        {
            std::ostringstream message;
            message << "JetCorrectorService[\"" << GetName() << "\"]::RegisterIOV: Run range (" <<
              minRun << ", " << maxRun << "), which is being registered under the label \"" <<
              label << "\" overlaps with a previously registered run range (" << iov.minRun <<
              ", " << maxRun << ").";
            throw std::runtime_error(message.str());
        }
    
    if (maxRun < minRun)
    {
        std::ostringstream message;
        message << "JetCorrectorService[\"" << GetName() << "\"]::RegisterIOV: Wrong ordering of "
          "boundaries of the run range (" << minRun << ", " << maxRun << ") registered under "
          "label \"" << label << "\".";
        throw std::runtime_error(message.str());
    }
    
    
    // Add a new IOV
    iovParams.emplace_back(minRun, maxRun);
    iovLabelMap[label] = iovParams.size() - 1;
}


void JetCorrectorService::SelectIOV(unsigned long run) const
{
    // Do nothing if there is only a match-all IOV and correctors have already been constructed
    if (matchAllMode and curIOV == 0)
        return;
    
    
    if (run == curRun)
        return;
    
    
    curRun = run;
    unsigned iovIndex = -1;
    
    for (unsigned i = 0; i < iovParams.size(); ++i)
        if (curRun >= iovParams[i].minRun and curRun <= iovParams[i].maxRun)
        {
            iovIndex = i;
            break;
        }
    
    if (iovIndex == unsigned(-1))
    {
        std::ostringstream message;
        message << "JetCorrectorService[\"" << GetName() << "\"]::SelectIOV: None of registered "
          "IOVs includes run " << run << ".";
        throw std::runtime_error(message.str());
    }
    
    if (iovIndex == curIOV)
    {
        // The run number has changed, but it still belongs to the same IOV. No need to update
        //anything.
        return;
    }
    
    
    // Update all JERC evaluators
    curIOV = iovIndex;
    
    auto nonConstThis = const_cast<JetCorrectorService *>(this);
    nonConstThis->UpdateJECEvaluator();
    nonConstThis->UpdateJECUncEvaluator();
    nonConstThis->UpdateJEREvaluator();
}


void JetCorrectorService::SetJECUncertainty(std::string const &iovLabel,
 std::string const &jecUncFile, std::string uncSource)
{
    SetJECUncertainty(iovLabel, jecUncFile, {uncSource});
}


void JetCorrectorService::SetJECUncertainty(std::string const &jecUncFile, std::string uncSource)
{
    SetJECUncertainty("", jecUncFile, {uncSource});
}


void JetCorrectorService::SetJER(std::string const &iovLabel, std::string const &jerSFFile,
  std::string const &jerMCFile)
{
    auto &iov = GetIOVByLabel(iovLabel);
    
    if (jerSFFile != "")
        iov.jerSFFile = FileInPath::Resolve("JERC", jerSFFile);
    else
        iov.jerSFFile = "";
    
    if (jerMCFile != "")
    {
        iov.jerMCFile = FileInPath::Resolve("JERC", jerMCFile);
        
        if (not rGen)
        {
            ROOTLock::Lock();
            rGen.reset(new TRandom3(0));
            ROOTLock::Unlock();
        }
    }
    else
        iov.jerMCFile = "";
}


void JetCorrectorService::SetJER(std::string const &jerSFFile, std::string const &jerMCFile)
{
    SetJER("", jerSFFile, jerMCFile);
}


JetCorrectorService::IOVParams &JetCorrectorService::GetIOVByLabel(std::string const &label)
{
    if (label == "")
    {
        // Special case of a match-all IOV. Make sure that no explicit IOV have been defined.
        if (not iovLabelMap.empty())
        {
            std::ostringstream message;
            message << "JetCorrectorService[\"" << GetName() << "\"]::GetIOVByLabel: Not possible "
              "to use the match-all implicit IOV when some explicit IOVs have been registered.";
            throw std::runtime_error(message.str());
        }
        
        // The match-all IOV might not have been created yet
        if (iovParams.empty())
        {
            iovParams.emplace_back(0, -1);
            matchAllMode = true;
        }
        
        return iovParams.front();
    }
    else
    {
        if (matchAllMode)
        {
            std::ostringstream message;
            message << "JetCorrectorService[\"" << GetName() << "\"]::GetIOVByLabel: Not possible "
              "to use together explicit IOVs with an implicit match-all one.";
            throw std::runtime_error(message.str());
        }
        
        
        auto const &res = iovLabelMap.find(label);
        
        if (res == iovLabelMap.end())
        {
            std::ostringstream message;
            message << "JetCorrectorService[\"" << GetName() << "\"]::GetIOVByLabel: IOV with "
              "label \"" << label << "\" has not been registered.";
            throw std::runtime_error(message.str());
        }
        
        return iovParams[res->second];
    }
}


void JetCorrectorService::UpdateJECEvaluator()
{
    auto const &jecFiles = iovParams[curIOV].jecFiles;
    
    if (jecFiles.size() > 0)
    {
        // Create an object that computes jet energy corrections. Code follows an example in [1].
        //[1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections?rev=136#JetEnCorFWLite
        std::vector<JetCorrectorParameters> jecParameters;
        
        for (auto const &jecFile: jecFiles)
            jecParameters.emplace_back(jecFile);
        
        jetEnergyCorrector.reset(new FactorizedJetCorrector(jecParameters));
    }
    else
        jetEnergyCorrector.reset();
}


void JetCorrectorService::UpdateJECUncEvaluator()
{
    auto const &iov = iovParams[curIOV];
    
    if (iov.jecUncFile != "")
    {
        // Create objects to compute JEC uncertainties. Code follows an example in [1].
        //[1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections?rev=136#JetCorUncertainties
        if (iov.jecUncSources.size() == 0)
            jecUncProviders.emplace_back(new JetCorrectionUncertainty(iov.jecUncFile));
        else
        {
            for (auto const &uncSource: iov.jecUncSources)
            {
                std::unique_ptr<JetCorrectorParameters> jecUncParams;
                
                try
                {
                    jecUncParams.reset(new JetCorrectorParameters(iov.jecUncFile, uncSource));
                }
                catch (std::runtime_error)
                {
                    std::ostringstream message;
                    message << "JetCorrectorService[\"" << GetName() <<
                      "\"]::UpdateJECUncEvaluator: Error while constructing JEC uncertainty \"" <<
                      uncSource << "\" from file \"" << iov.jecUncFile << "\". The file might not "
                      "contain definition for the requested uncertainty.";
                    throw std::runtime_error(message.str());
                }
                
                jecUncProviders.emplace_back(new JetCorrectionUncertainty(*jecUncParams));
            }
        }
    }
    else
        jecUncProviders.clear();
}


void JetCorrectorService::UpdateJEREvaluator()
{
    auto const &iov = iovParams[curIOV];
    
    if (iov.jerSFFile != "")
        jerSFProvider.reset(
          new JME::JetResolutionScaleFactor(iov.jerSFFile));
    else
        jerSFProvider.reset();
    
    if (iov.jerMCFile != "")
        jerProvider.reset(new JME::JetResolution(iov.jerMCFile));
    else
        jerProvider.reset();
}

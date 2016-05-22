#include <mensura/extensions/JetCorrectorService.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/core/PhysicsObjects.hpp>
#include <mensura/core/ROOTLock.hpp>

#include <mensura/external/JERC/JetCorrectorParameters.hpp>

#include <cmath>
#include <stdexcept>


JetCorrectorService::JetCorrectorService(std::string const name /*= "JetCorrector"*/):
    Service(name)
{}


JetCorrectorService::JetCorrectorService(JetCorrectorService const &src):
    Service(src),
    jecFiles(src.jecFiles),
    jecUncFile(src.jecUncFile), jecUncSources(src.jecUncSources),
    jerSFFile(src.jerSFFile), jerMCFile(src.jerMCFile)
{
    // Construct all objects to evaluate JEC and impact of JER
    CreateJECEvaluator();
    CreateJECUncEvaluator();
    CreateJEREvaluator();
    
    
    // Create a random-number generator if needed. Cannot share the same generator between copies
    //because generation of random numbers is not thread-safe
    if (jerMCFile != "")
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
    // Variable that will accumulate the total correction factor
    double corrFactor = 1.;
    
    // Running jet pt with all corrections considered up to the current moment applied
    double curCorrPt = jet.Pt();    
    
    
    // Evaluate nominal jet energy correction
    if (jetEnergyCorrector)
    {
        curCorrPt = jet.RawP4().Pt();
        
        jetEnergyCorrector->setJetEta(jet.Eta());
        jetEnergyCorrector->setJetPt(curCorrPt);
        jetEnergyCorrector->setJetA(jet.Area());
        jetEnergyCorrector->setRho(rho);
        
        double const jecFactor = jetEnergyCorrector->getCorrection();
        
        corrFactor *= jecFactor;
        curCorrPt *= jecFactor;
    }
    
    // At this point curCorrPt is assumed to include full nominal JEC, either applied in this
    //method or set up when the jet was created
    
    
    // Evaluate systematical variation for JEC
    if (syst == SystType::JEC)
    {
        // Sanity check
        if (jecUncProviders.size() == 0)
            throw std::logic_error("JetCorrectorService::Eval: Cannot evaluate JEC systematics "
              "because no uncertainties have been specified.");
        
        
        double const jecUncertainty = EvalJECUnc(curCorrPt, jet.Eta());
        
        if (direction == SystService::VarDirection::Up)
        {
            corrFactor *= (1. + jecUncertainty);
            curCorrPt *= (1. + jecUncertainty);
        }
        else if (direction == SystService::VarDirection::Down)
        {
            corrFactor *= (1. - jecUncertainty);
            curCorrPt *= (1. - jecUncertainty);
        }
    }
    
    
    // Sanity check before JER smearing
    if (syst == SystType::JER and direction != SystService::VarDirection::Undefined and
      not jerSFProvider)
        throw std::logic_error("JetCorrectorService::Eval: Cannot evaluate JER systematics because "
          "JER scale factors have not been specified.");
    
    
    // Evaluate JER smearing
    if (jerSFProvider)
    {
        // Find data/MC scale factor for pt resolution for the current jet
        double jerSF = 0.;
        
        switch (direction)
        {
            case SystService::VarDirection::Up:
                jerSF = jerSFProvider->getScaleFactor({{JME::Binning::JetEta, jet.Eta()}},
                  Variation::UP);
                break;
            
            case SystService::VarDirection::Down:
                jerSF = jerSFProvider->getScaleFactor({{JME::Binning::JetEta, jet.Eta()}},
                  Variation::DOWN);
                break;
            
            default:
                jerSF = jerSFProvider->getScaleFactor({{JME::Binning::JetEta, jet.Eta()}},
                  Variation::NOMINAL);
        }
        
        
        // Depending on the presence of a matched GEN-level jet, perform deterministic or
        //stochastic smearing
        GenJet const *genJet = jet.MatchedGenJet();
        
        if (genJet)
        {
            // Smearing is done as here [1]
            //[1] https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_8/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h#L236-L237
            double const jerFactor = 1. + (jerSF - 1.) * (curCorrPt - genJet->Pt()) / curCorrPt;
            
            corrFactor *= jerFactor;
            // No need to update curCorrPt since it will not be used anymore
        }
        else if (jerProvider)
        {
            // Follow the same approach as here [1]
            //[1] https://github.com/cms-sw/cmssw/blob/CMSSW_8_0_8/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h#L244_L250
            double const ptResolution =
              jerProvider->getResolution({{JME::Binning::JetPt, curCorrPt},
              {JME::Binning::JetEta, jet.Eta()}, {JME::Binning::Rho, rho}});
            double const jerFactor = 1. + rGen->Gaus(0., ptResolution) *
              std::sqrt(std::max(std::pow(jerSF, 2) - 1., 0.)) / curCorrPt;
            
            corrFactor *= jerFactor;
            // No need to update curCorrPt since it will not be used anymore
        }
    }
    
    
    return corrFactor;
}


double JetCorrectorService::EvalJECUnc(double const corrPt, double const eta) const
{
    if (jecUncProviders.size() == 1)
    {
        // Consider the case of a single uncertainty specially in order to avoid unnecessary
        //computation of sqrt(unc^2)
        auto &acc = jecUncProviders.front();
        acc->setJetEta(eta);
        acc->setJetPt(corrPt);
        
        return acc->getUncertainty(true);
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
        
        return std::sqrt(unc2);
    }
}


double JetCorrectorService::operator()(Jet const &jet, double rho, SystType syst /*= SystType::None*/,
  SystService::VarDirection direction /*= SystService::VarDirection::Undefined*/) const
{
    return Eval(jet, rho, syst, direction);
}


void JetCorrectorService::SetJEC(std::initializer_list<std::string> const &jecFiles_)
{
    jecFiles.clear();
    
    for (auto const &jecFile: jecFiles_)
        jecFiles.emplace_back(FileInPath::Resolve("JERC", jecFile));
    
    CreateJECEvaluator();
}


void JetCorrectorService::SetJECUncertainty(std::string const &jecUncFile_,
  std::initializer_list<std::string> uncSources /*= {}*/)
{
    if (jecUncFile_ != "")
    {
        jecUncFile = FileInPath::Resolve("JERC", jecUncFile_);
        jecUncSources = std::vector<std::string>(uncSources);
    }
    else
    {
        jecUncFile = "";
        jecUncSources.clear();
    }
    
    CreateJECUncEvaluator();
}


void JetCorrectorService::SetJER(std::string const &jerSFFile_, std::string const &jerMCFile_,
  unsigned long seed /*= 0*/)
{
    if (jerSFFile_ != "")
        jerSFFile = FileInPath::Resolve("JERC", jerSFFile_);
    else
        jerSFFile = "";
    
    if (jerMCFile_ != "")
    {
        jerMCFile = FileInPath::Resolve("JERC", jerMCFile_);
        
        ROOTLock::Lock();
        rGen.reset(new TRandom3(seed));
        ROOTLock::Unlock();
    }
    else
        jerMCFile = "";
    
    CreateJEREvaluator();
}


void JetCorrectorService::CreateJECEvaluator()
{
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


void JetCorrectorService::CreateJECUncEvaluator()
{
    if (jecUncFile != "")
    {
        // Create objects to compute JEC uncertainties. Code follows an example in [1].
        //[1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections?rev=136#JetCorUncertainties
        if (jecUncSources.size() == 0)
            jecUncProviders.emplace_back(new JetCorrectionUncertainty(jecUncFile));
        else
        {
            for (auto const &uncSource: jecUncSources)
                jecUncProviders.emplace_back(
                  new JetCorrectionUncertainty(JetCorrectorParameters(jecUncFile, uncSource)));
        }
    }
    else
        jecUncProviders.clear();
}


void JetCorrectorService::CreateJEREvaluator()
{
    if (jerSFFile != "")
        jerSFProvider.reset(
          new JME::JetResolutionScaleFactor(jerSFFile));
    else
        jerSFProvider.reset();
    
    if (jerMCFile != "")
        jerProvider.reset(new JME::JetResolution(jerMCFile));
    else
        jerProvider.reset();
}
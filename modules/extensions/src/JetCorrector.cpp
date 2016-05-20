#include <mensura/extensions/JetCorrector.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/core/ROOTLock.hpp>

#include <mensura/external/JERC/JetCorrectorParameters.hpp>

#include <cmath>
#include <stdexcept>


using namespace std;


JetCorrector::JetCorrector() noexcept:
    rGen(0)
{}


// JetCorrector::JetCorrector(std::initializer_list<std::string> const &dataFilesJEC_,
//   std::string const &dataFileJECUncertainty_ /*= ""*/, std::string const &dataFileJER_ /*= ""*/):
//     dataFilesJEC(dataFilesJEC_),
//     dataFileJECUncertainty(dataFileJECUncertainty_),
//     dataFileJER(dataFileJER_)
// {}


// JetCorrector *JetCorrector::Clone() const
// {
//     return new JetCorrector(dataFilesJEC, dataFileJECUncertainty, dataFileJER);
// }


void JetCorrector::SetJEC(std::initializer_list<std::string> const &jecFiles)
{
    // Create an object that computes jet energy corrections. Code follows an example in [1].
    //[1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections?rev=136#JetEnCorFWLite
    std::vector<JetCorrectorParameters> jecParameters;
    
    for (auto const &jecFile: jecFiles)
        jecParameters.emplace_back(FileInPath::Resolve("JERC", jecFile));
    
    jetEnergyCorrector.reset(new FactorizedJetCorrector(jecParameters));
}


void JetCorrector::SetJECUncertainty(std::string const &jecUncFile,
  std::initializer_list<std::string> uncSources /*= {}*/)
{
    // Create objects to compute JEC uncertainties. Code follows an example in [1].
    //[1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections?rev=136#JetCorUncertainties
    std::string const &resolvedPath = FileInPath::Resolve("JERC", jecUncFile);
    
    if (uncSources.size() == 0)
        jecUncAccessors.emplace_back(new JetCorrectionUncertainty(resolvedPath));
    else
    {
        for (auto const &uncSource: uncSources)
            jecUncAccessors.emplace_back(
              new JetCorrectionUncertainty(JetCorrectorParameters(resolvedPath, uncSource)));
    }
}


void JetCorrector::SetJER(std::string const &jerSFFile, std::string const &jerMCFile)
{
    if (jerSFFile != "")
        jerSFProvider.reset(
          new JME::JetResolutionScaleFactor(FileInPath::Resolve("JERC", jerSFFile)));
    
    if (jerMCFile != "")
        jerProvider.reset(new JME::JetResolution(FileInPath::Resolve("JERC", jerMCFile)));
}


double JetCorrector::EvalJECUnc(double const corrPt, double const eta) const
{
    if (jecUncAccessors.size() == 1)
    {
        // Consider the case of a single uncertainty specially in order to avoid unnecessary
        //computation of sqrt(unc^2)
        auto &acc = jecUncAccessors.front();
        acc->setJetEta(eta);
        acc->setJetPt(corrPt);
        
        return acc->getUncertainty(true);
    }
    else
    {
        double unc2 = 0.;
        
        for (auto &acc: jecUncAccessors)
        {
            acc->setJetEta(eta);
            acc->setJetPt(corrPt);
            
            unc2 += std::pow(acc->getUncertainty(true), 2);
        }
        
        return std::sqrt(unc2);
    }
}


double JetCorrector::Eval(Jet const &jet, double rho, SystType syst /*= SystType::None*/,
  SystService::VarDirection direction /*= SystService::VarDirection::Undefined*/) const
{
    double fullFactor = 1.;
    TLorentzVector const &rawP4 = jet.RawP4();
    
    
    // Evaluate the total jet energy correction
    jetEnergyCorrector->setJetEta(rawP4.Eta());
    jetEnergyCorrector->setJetPt(rawP4.Pt());
    jetEnergyCorrector->setJetA(jet.Area());
    jetEnergyCorrector->setRho(rho);
    
    double jecFactor = jetEnergyCorrector->getCorrection();
    double const corrPt = rawP4.Pt() * jecFactor;
    
    
    // Evaluate systematical variation for JEC
    if (syst == SystType::JEC)
    {
        // First a sanity check
        if (jecUncAccessors.size() == 0)
            throw std::logic_error("JetCorrector::Eval: Trying to evaluate JEC systematics while "
              "JEC uncertainties have not been set up.");
        
        
        double const jecUncertainty = EvalJECUnc(corrPt, rawP4.Eta());
        
        if (direction == SystService::VarDirection::Up)
            jecFactor *= (1. + jecUncertainty);
        else if (direction == SystService::VarDirection::Down)
            jecFactor *= (1. - jecUncertainty);
    }
    
    
    fullFactor *= jecFactor;
    
    
    // A sanity check before JER smearing
    if (syst == SystType::JER and direction != SystService::VarDirection::Undefined and
      not jerSFProvider)
        throw std::logic_error("JetCorrector::Eval: Trying to evaluate JER systematics while "
          "JER scale factors have not been set up.");
    
    
    // Apply JER smearing
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
            // double const jerFactor = 1. + (jerSF - 1.) * (corrPt - genJet->Pt()) / corrPt;
            double const jerFactor = 1. +
              (jerSF - 1.) * (rawP4.E() * jecFactor - genJet->P4().E()) / (rawP4.E() * jecFactor);
            
            fullFactor *= jerFactor;
        }
        else if (jerProvider)
        {
            double const ptResolution = jerProvider->getResolution({{JME::Binning::JetPt, corrPt},
              {JME::Binning::JetEta, jet.Eta()}, {JME::Binning::Rho, rho}});
            double const jerFactor = 1. + rGen.Gaus(0., ptResolution) *
              std::sqrt(std::max(std::pow(jerSF, 2) - 1., 0.)) / corrPt;
            
            fullFactor *= jerFactor;
        }
    }
    
    
    return fullFactor;
}


double JetCorrector::operator()(Jet const &jet, double rho, SystType syst /*= SystType::None*/,
  SystService::VarDirection direction /*= SystService::VarDirection::Undefined*/) const
{
    return Eval(jet, rho, syst, direction);
}


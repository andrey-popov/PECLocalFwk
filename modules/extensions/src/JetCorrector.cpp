#include <mensura/extensions/JetCorrector.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/core/ROOTLock.hpp>

#include <mensura/external/JERC/JetCorrectorParameters.hpp>

#include <stdexcept>


using namespace std;


JetCorrector::JetCorrector() noexcept
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


void JetCorrector::SetJERFile(string const &jerFile)
{
    jerAccessor.reset(new JetResolutionFactor(jerFile));
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
    
    
    // Evaluate systematical variation for JEC
    if (syst == SystType::JEC)
    {
        // First a sanity check
        if (jecUncAccessors.size() == 0)
            throw logic_error("JetCorrector::Eval: Trying to evaluate JEC systematics while "
             "JEC uncertainties have not been set up.");
        
        
        double const jecUncertainty = EvalJECUnc(rawP4.Pt() * jecFactor, rawP4.Eta());
        
        if (direction == SystService::VarDirection::Up)
            jecFactor *= (1. + jecUncertainty);
        else if (direction == SystService::VarDirection::Down)
            jecFactor *= (1. - jecUncertainty);
    }
    
    
    fullFactor *= jecFactor;
    
    
    // A sanity check before JER smearing
    if (not jerAccessor and syst == SystType::JER and
      direction != SystService::VarDirection::Undefined)
        throw logic_error("JetCorrector::Correct: Trying to evaluate JER systematics while "
         "data file with parameters for JER has not been provided.");
    
    
    // Evaluate JER smearing
    if (jerAccessor)
    {
        JetResolutionFactor::SystVariation jerSyst = JetResolutionFactor::SystVariation::Nominal;
        
        if (syst == SystType::JER)
        {
            if (direction == SystService::VarDirection::Up)
                jerSyst = JetResolutionFactor::SystVariation::Up;
            else if (direction == SystService::VarDirection::Down)
                jerSyst = JetResolutionFactor::SystVariation::Down;
        }
        
        
        fullFactor *= jerAccessor->GetFactor(jet, jerSyst);
    }
    
    
    return fullFactor;
}


double JetCorrector::operator()(Jet const &jet, double rho, SystType syst /*= SystType::None*/,
  SystService::VarDirection direction /*= SystService::VarDirection::Undefined*/) const
{
    return Eval(jet, rho, syst, direction);
}


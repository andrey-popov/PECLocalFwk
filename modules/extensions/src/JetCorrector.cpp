#include <mensura/extensions/JetCorrector.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/core/ROOTLock.hpp>

#include <mensura/external/JERC/JetCorrectorParameters.hpp>

#include <stdexcept>


using namespace std;


JetCorrector::JetCorrector() noexcept
{}


JetCorrector::JetCorrector(vector<string> const &dataFilesJEC_,
 string const &dataFileJECUncertainty_ /*= ""*/, string const &dataFileJER_ /*= ""*/) noexcept:
    dataFilesJEC(dataFilesJEC_),
    dataFileJECUncertainty(dataFileJECUncertainty_),
    dataFileJER(dataFileJER_)
{}


JetCorrectorInterface *JetCorrector::Clone() const noexcept
{
    return new JetCorrector(dataFilesJEC, dataFileJECUncertainty, dataFileJER);
}


void JetCorrector::AddJECLevel(std::string const &dataFile) noexcept
{
    dataFilesJEC.push_back(dataFile);
}


void JetCorrector::SetJECUncertainty(std::string const &dataFile) noexcept
{
    dataFileJECUncertainty = dataFile;
}


void JetCorrector::SetJERFile(string const &dataFile) noexcept
{
    dataFileJER = dataFile;
}


void JetCorrector::Init()
{
    // Create an object to perform jet energy corrections. Code follows an example in [1]. The block
    //is locked because the classes to deal with JEC construct some ROOT entities
    //[1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetEnCorFWLite
    ROOTLock::Lock();
    
    vector<JetCorrectorParameters> jecParameters;
    
    for (string const &dataFile: dataFilesJEC)
        jecParameters.emplace_back(FileInPath::Resolve(dataFile));
    
    jetEnergyCorrector.reset(new FactorizedJetCorrector(jecParameters));
    
    
    // Create an object to evaluate JEC uncertainties if requested. Follows an example in [1]
    //[1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetCorUncertainties
    if (dataFileJECUncertainty.length() > 0)
        jecUncertaintyAccessor.reset(
         new JetCorrectionUncertainty(FileInPath::Resolve(dataFileJECUncertainty)));
    
    ROOTLock::Unlock();
    
    
    // Create an object to evaluate JER scale factor
    if (dataFileJER.length() > 0)
        jerAccessor.reset(new JetResolutionFactor(dataFileJER));
}


void JetCorrector::Correct(Jet &jet, double rho, SystType syst /*= SystType::None*/,
  SystService::VarDirection direction /*= SystService::VarDirection::Undefined*/) const
{
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
        if (not jecUncertaintyAccessor)
            throw logic_error("JetCorrector::Correct: Trying to evaluate JEC systematics while "
             "JEC uncertainties have not been set up.");
        
        
        jecUncertaintyAccessor->setJetEta(rawP4.Eta());
        jecUncertaintyAccessor->setJetPt(rawP4.Pt() * jecFactor);  // use corrected pt
        
        double const jecUncertainty = jecUncertaintyAccessor->getUncertainty(true);
        
        if (direction == SystService::VarDirection::Up)
            jecFactor *= (1. + jecUncertainty);
        else if (direction == SystService::VarDirection::Down)
            jecFactor *= (1. - jecUncertainty);
    }
    
    
    // Correct the jet for JEC
    jet.SetCorrectedP4(rawP4 * jecFactor, 1. / jecFactor);
    
    
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
        
        
        double const jerFactor = jerAccessor->GetFactor(jet, jerSyst);
        
        double const factor = jecFactor * jerFactor;
        jet.SetCorrectedP4(rawP4 * factor, 1. / factor);
    }
}

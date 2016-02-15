#include <PECFwk/extensions/JetCorrector.hpp>

#include <PECFwk/core/FileInPath.hpp>
#include <PECFwk/core/ROOTLock.hpp>

#include <PECFwk/external/JERC/JetCorrectorParameters.hpp>

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
    // An object to resolve file paths
    FileInPath pathResolver;
    
    
    // Create an object to perform jet energy corrections. Code follows an example in [1]. The block
    //is locked because the classes to deal with JEC construct some ROOT entities
    //[1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetEnCorFWLite
    ROOTLock::Lock();
    
    vector<JetCorrectorParameters> jecParameters;
    
    for (string const &dataFile: dataFilesJEC)
        jecParameters.emplace_back(pathResolver.Resolve(dataFile));
    
    jetEnergyCorrector.reset(new FactorizedJetCorrector(jecParameters));
    
    
    // Create an object to evaluate JEC uncertainties if requested. Follows an example in [1]
    //[1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetCorUncertainties
    if (dataFileJECUncertainty.length() > 0)
        jecUncertaintyAccessor.reset(
         new JetCorrectionUncertainty(pathResolver.Resolve(dataFileJECUncertainty)));
    
    ROOTLock::Unlock();
    
    
    // Create an object to evaluate JER scale factor
    if (dataFileJER.length() > 0)
        jerAccessor.reset(new JetResolutionFactor(dataFileJER));
}


void JetCorrector::Correct(Jet &jet, double rho, SystVariation syst /*= SystVariation()*/) const
{
    TLorentzVector const &rawP4 = jet.RawP4();
    
    
    // Evaluate the total jet energy correction
    jetEnergyCorrector->setJetEta(rawP4.Eta());
    jetEnergyCorrector->setJetPt(rawP4.Pt());
    jetEnergyCorrector->setJetA(jet.Area());
    jetEnergyCorrector->setRho(rho);
    
    double jecFactor = jetEnergyCorrector->getCorrection();
    
    
    // Evaluate systematical variation for JEC
    if (syst.type == SystTypeAlgo::JEC)
    {
        // First a sanity check
        if (not jecUncertaintyAccessor)
            throw logic_error("JetCorrector::Correct: Trying to evaluate JEC systematics while "
             "JEC uncertainties have not been set up.");
        
        
        jecUncertaintyAccessor->setJetEta(rawP4.Eta());
        jecUncertaintyAccessor->setJetPt(rawP4.Pt() * jecFactor);  // use corrected pt
        
        double const jecUncertainty = jecUncertaintyAccessor->getUncertainty(true);
        
        jecFactor *= (1. + syst.direction * jecUncertainty);
    }
    
    
    // Correct the jet for JEC
    jet.SetCorrectedP4(rawP4 * jecFactor, 1. / jecFactor);
    
    
    // A sanity check before JER smearing
    if (not jerAccessor and syst.type == SystTypeAlgo::JER and syst.direction != 0)
        throw logic_error("JetCorrector::Correct: Trying to evaluate JER systematics while "
         "data file with parameters for JER has not been provided.");
    
    
    // Evaluate JER smearing
    if (jerAccessor)
    {
        JetResolutionFactor::SystVariation jerSyst = JetResolutionFactor::SystVariation::Nominal;
        
        if (syst.type == SystTypeAlgo::JER)
        {
            if (syst.direction > 0)
                jerSyst = JetResolutionFactor::SystVariation::Up;
            else if (syst.direction < 0)
                jerSyst = JetResolutionFactor::SystVariation::Down;
        }
        
        
        double const jerFactor = jerAccessor->GetFactor(jet, jerSyst);
        
        double const factor = jecFactor * jerFactor;
        jet.SetCorrectedP4(rawP4 * factor, 1. / factor);
    }
}

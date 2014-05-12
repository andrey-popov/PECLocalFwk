#include <JetCorrector.hpp>

#include <FileInPath.hpp>

#include <external/JEC/include/JetCorrectorParameters.hpp>

#include <stdexcept>


using namespace std;


JetCorrector::JetCorrector() noexcept
{}


JetCorrector::JetCorrector(vector<string> const &srcFilesJEC_, string const &srcFileJECUncertainty_)
 noexcept:
    srcFilesJEC(srcFilesJEC_),
    srcFileJECUncertainty(srcFileJECUncertainty_)
{}


JetCorrectorInterface *JetCorrector::Clone() const noexcept
{
    return new JetCorrector(srcFilesJEC, srcFileJECUncertainty);
}


void JetCorrector::AddJECLevel(std::string const &srcFile) noexcept
{
    srcFilesJEC.push_back(srcFile);
}


void JetCorrector::SetJECUncertainty(std::string const &srcFile) noexcept
{
    srcFileJECUncertainty = srcFile;
}


void JetCorrector::Init()
{
    // An object to resolve file paths
    FileInPath pathResolver;
    
    
    // Create an object to perform jet energy corrections. Code follows an example in [1]
    //[1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetEnCorFWLite
    vector<JetCorrectorParameters> jecParameters;
    
    for (string const &srcFile: srcFilesJEC)
        jecParameters.emplace_back(pathResolver.Resolve(srcFile));
    
    jetEnergyCorrector.reset(new FactorizedJetCorrector(jecParameters));
    
    
    // Create an object to evaluate JEC uncertainties if requested. Follows an example in [1]
    //[1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/WorkBookJetEnergyCorrections#JetCorUncertainties
    if (srcFileJECUncertainty.length() > 0)
        jecUncertaintyAccessor.reset(
         new JetCorrectionUncertainty(pathResolver.Resolve(srcFileJECUncertainty)));
}


void JetCorrector::Correct(Jet &jet, double rho, SystVariation syst /*= SystVariation()*/)
{
    // Evaluate the total jet energy correction
    jetEnergyCorrector->setJetEta(jet.Eta());
    jetEnergyCorrector->setJetPt(jet.Pt());
    jetEnergyCorrector->setJetA(jet.Area());
    jetEnergyCorrector->setRho(rho);
    
    double const jecFactor = jetEnergyCorrector->getCorrection();
    
    
    // Calculate JEC uncertainty
    double jecUncertainty = 0.;
    
    if (syst.type == SystTypeAlgo::JEC)
    {
        // First a sanity check
        if (not jecUncertaintyAccessor)
            throw logic_error("JetCorrector::Correct: Trying to evaluate JEC systematics while "
             "JEC uncertainties have not been set up.");
        
        jecUncertaintyAccessor->setJetEta(jet.Eta());
        jecUncertaintyAccessor->setJetPt(jet.Pt() * jecFactor);  // use corrected pt
        
        jecUncertainty = jecUncertaintyAccessor->getUncertainty(true);
    }
    
    
    // Evaluate JER smearing
    double jerFactor = 1.;  // a placeholder
    
    
    // Update jet four-momentum
    double const factor = jecFactor * (1. + syst.direction * jecUncertainty) * jerFactor;
    jet.SetCorrectedP4(jet.P4() * factor, 1. / factor);
}

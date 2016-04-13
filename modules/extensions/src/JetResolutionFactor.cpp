#include <mensura/extensions/JetResolutionFactor.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/core/ROOTLock.hpp>

#include <TFile.h>


using namespace std;


JetResolutionFactor::JetResolutionFactor(string const &dataFileName_) noexcept:
    dataFileName(dataFileName_)
{
    // Read the JER histogram from the data file
    FileInPath pathResolver;
    
    ROOTLock::Lock();
    
    TFile dataFile(pathResolver.Resolve(dataFileName).c_str());
    jerHist.reset(dynamic_cast<TH2D *>(dataFile.Get("pfJetResolutionMCtoDataCorrLUT")));
    jerHist->SetDirectory(nullptr);
    dataFile.Close();
    
    ROOTLock::Unlock();
}


JetResolutionFactor::JetResolutionFactor(JetResolutionFactor const &src) noexcept:
    dataFileName(src.dataFileName)
{
    // Copy the JER histogram
    ROOTLock::Lock();
    
    jerHist.reset(new TH2D(*src.jerHist.get()));
    
    ROOTLock::Unlock();
}


double JetResolutionFactor::GetFactor(Jet const &jet,
 SystVariation syst /*= SystVariation::Nominal*/) const noexcept
{
    // Do not smear the jet if it does not have a generator-level match
    if (not jet.MatchedGenJet())
        return 1.;
    
    
    // Apart from that, the smearing factor is calculated as in SmearedJetProducerT [1]
    //[1] https://github.com/cms-sw/cmssw/blob/CMSSW_5_3_11/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h
    
    double const corrPt = jet.Pt();
    double const absEta = fabs(jet.Eta());
    
    
    // Make sure the jet lies within the range of histogram. Unfortunately, under/overflow bins are
    //not filled properly
    if (absEta < jerHist->GetXaxis()->GetXmin() or absEta >= jerHist->GetXaxis()->GetXmax() or
     corrPt < jerHist->GetYaxis()->GetXmin() or corrPt >= jerHist->GetYaxis()->GetXmax())
        // Do not smear jets in under/overflow bins
        return 1.;
    
    
    // Extract the smear factor from the histogram
    int const bin = jerHist->FindFixBin(absEta, corrPt);
    double smearFactor = jerHist->GetBinContent(bin);
    
    if (syst != SystVariation::Nominal)
    {
        double const smearFactorErr = jerHist->GetBinError(bin);
        
        smearFactor += smearFactorErr * int(syst);
    }
    
    
    // Calculate the scale factor for jet momentum
    double const scaleFactor = 1. +
     (smearFactor - 1.) * (jet.E() - jet.MatchedGenJet()->E()) / max(jet.E(), jet.RawP4().E());
    //^ Here the definition from CMSSW_5_3_11 is used, but later in the branch 53X the denominator
    //was simplified to just jet.E()
    
    return scaleFactor;
}

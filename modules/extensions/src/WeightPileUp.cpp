#include <PECFwk/extensions/WeightPileUp.hpp>

#include <PECFwk/core/FileInPath.hpp>
#include <PECFwk/core/PileUpReader.hpp>
#include <PECFwk/core/Processor.hpp>
#include <PECFwk/core/ROOTLock.hpp>

#include <cstdlib>
#include <stdexcept>


WeightPileUp::WeightPileUp(std::string const &name, std::string const &dataPUFileName,
  std::string const &mcPUFileName, double systError_):
    AnalysisPlugin(name),
    puPluginName("PileUp"), puPlugin(nullptr),
    systError(systError_)
{
    // Target distribution
    ReadTargetDistribution(dataPUFileName);
    
    
    // File with distributions as generated
    FileInPath pathResolver;
    
    ROOTLock::Lock();
    mcPUFile.reset(new TFile((pathResolver.Resolve("PileUp/", mcPUFileName)).c_str()));
    ROOTLock::Unlock();
}


WeightPileUp::WeightPileUp(std::string const &name, std::string const &dataPUFileName,
  double systError_):
    AnalysisPlugin(name),
    puPluginName("PileUp"), puPlugin(nullptr),
    systError(systError_)
{
    ReadTargetDistribution(dataPUFileName);
}


WeightPileUp::WeightPileUp(std::string const &dataPUFileName, double systError_):
    AnalysisPlugin("WeightPileUp"),
    puPluginName("PileUp"), puPlugin(nullptr),
    systError(systError_)
{
    ReadTargetDistribution(dataPUFileName);
}


WeightPileUp::~WeightPileUp() noexcept
{}


void WeightPileUp::BeginRun(Dataset const &)
{
    // Save pointer to pile-up reader
    puPlugin = dynamic_cast<PileUpReader const *>(
      GetMaster().GetPluginBefore(puPluginName, GetName()));
    
    
    // Load the distribution of expected pile-up as generated for the current dataset. (This is)
    //not implemented yet.)
    if (mcPUFile)
    {
        // Check if a MC-truth histogram is available for the new dataset and update mcPUHist if
        //needed. Return on success. Make sure to rescale the histograms with "weight" option
        
        throw std::logic_error("WeightPileUp::BeginRun: Reweighting with actual MC pile-up "
         "distributions described in a file is not implemented yet.");
    }
    
    
    // If the control reaches this point, either file with MC-truth histograms has not been
    //specified or no dataset-specific histogram is available for the current file. Anyway the
    //nominal pile-up distribution should be used
    if (not mcPUHist or std::string(mcPUHist->GetName()) != "nominal")
    {
        // MC distribution for Summer2012, S10
        //[1] https://twiki.cern.ch/twiki/bin/view/CMS/Pileup_MC_Gen_Scenarios
        std::vector<double> pileUpTruthHist = {2.560E-06, 5.239E-06, 1.420E-05, 5.005E-05, 1.001E-04,
         2.705E-04, 1.999E-03, 6.097E-03, 1.046E-02, 1.383E-02, 1.685E-02, 2.055E-02, 2.572E-02,
         3.262E-02, 4.121E-02, 4.977E-02, 5.539E-02, 5.725E-02, 5.607E-02, 5.312E-02, 5.008E-02,
         4.763E-02, 4.558E-02, 4.363E-02, 4.159E-02, 3.933E-02, 3.681E-02, 3.406E-02, 3.116E-02,
         2.818E-02, 2.519E-02, 2.226E-02, 1.946E-02, 1.682E-02, 1.437E-02, 1.215E-02, 1.016E-02,
         8.400E-03, 6.873E-03, 5.564E-03, 4.457E-03, 3.533E-03, 2.772E-03, 2.154E-03, 1.656E-03,
         1.261E-03, 9.513E-04, 7.107E-04, 5.259E-04, 3.856E-04, 2.801E-04, 2.017E-04, 1.439E-04,
         1.017E-04, 7.126E-05, 4.948E-05, 3.405E-05, 2.322E-05, 1.570E-05, 5.005E-06};
        
        
        // Create a new MC pile-up histogram
        ROOTLock::Lock();
        mcPUHist.reset(new TH1D("nominal", "", pileUpTruthHist.size(),
         0., pileUpTruthHist.size()));
        mcPUHist->SetDirectory(nullptr);
        ROOTLock::Unlock();
        
        // Fill it
        for (unsigned bin = 1; bin <= pileUpTruthHist.size(); ++bin)
            mcPUHist->SetBinContent(bin, pileUpTruthHist.at(bin - 1));
        
        // Normalize the histogram to get a probability density and adjust the over/underflow bins
        mcPUHist->Scale(1. / mcPUHist->Integral(0, -1), "width");
        mcPUHist->SetBinContent(0, 0.);
        mcPUHist->SetBinContent(mcPUHist->GetNbinsX() + 1, 0.);
    }
}


Plugin *WeightPileUp::Clone() const
{
    return new WeightPileUp(*this);
}


WeightPileUp::Weights const &WeightPileUp::GetWeights() const
{
    return weights;
}


bool WeightPileUp::ProcessEvent()
{
    // Read the expected number of pile-up events
    double const nTruth = puPlugin->GetExpectedPileUp();
    
    
    // Find probability according to MC histogram
    unsigned bin = mcPUHist->FindFixBin(nTruth);
    double const mcProb = mcPUHist->GetBinContent(bin);
    
    if (mcProb <= 0.)
        weights = {0., 0., 0.};
    
    
    // Calculate the weights
    bin = dataPUHist->FindFixBin(nTruth);
    weights.central = dataPUHist->GetBinContent(bin) / mcProb;
    
    bin = dataPUHist->FindFixBin(nTruth * (1. + systError));
    weights.up = dataPUHist->GetBinContent(bin) / mcProb * (1. + systError);
    //^ The last multiplier is needed to correct for the total normalisation due to rescale in
    //the variable of integration. Same applies to the down weight below
    
    bin = dataPUHist->FindFixBin(nTruth * (1. - systError));
    weights.down = dataPUHist->GetBinContent(bin) / mcProb * (1. - systError);
    
    
    // This plugin does not perform any filtering, so ProcessEvent always returns true
    return true;
}


void WeightPileUp::ReadTargetDistribution(std::string const &dataPUFileName)
{
    FileInPath pathResolver;
    
    
    // ROOT objects are created below. Mark it as a critical block
    ROOTLock::Lock();
    
    // Read the target (data) pile-up distribution
    TFile dataPUFile((pathResolver.Resolve("PileUp/", dataPUFileName)).c_str());
    dataPUHist.reset(dynamic_cast<TH1 *>(dataPUFile.Get("pileup")));
    
    // Make sure the histogram is not associated to a file
    dataPUHist->SetDirectory(nullptr);
    
    // Normalize it to get a probability density and adjust the over/underflow bins
    dataPUHist->Scale(1. / dataPUHist->Integral(0, -1), "width");
    dataPUHist->SetBinContent(0, 0.);
    dataPUHist->SetBinContent(dataPUHist->GetNbinsX() + 1, 0.);
    
    dataPUFile.Close();
    
    // End of critical block
    ROOTLock::Unlock();
}

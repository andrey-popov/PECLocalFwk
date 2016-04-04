#include <PECFwk/extensions/PileUpWeight.hpp>

#include <PECFwk/core/FileInPath.hpp>
#include <PECFwk/core/PileUpReader.hpp>
#include <PECFwk/core/Processor.hpp>
#include <PECFwk/core/ROOTLock.hpp>

#include <cstdlib>
#include <stdexcept>


PileUpWeight::PileUpWeight(std::string const &name, std::string const &dataPUFileName,
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


PileUpWeight::PileUpWeight(std::string const &name, std::string const &dataPUFileName,
  double systError_):
    AnalysisPlugin(name),
    puPluginName("PileUp"), puPlugin(nullptr),
    systError(systError_)
{
    ReadTargetDistribution(dataPUFileName);
}


PileUpWeight::PileUpWeight(std::string const &dataPUFileName, double systError_):
    AnalysisPlugin("PileUpWeight"),
    puPluginName("PileUp"), puPlugin(nullptr),
    systError(systError_)
{
    ReadTargetDistribution(dataPUFileName);
}


PileUpWeight::~PileUpWeight() noexcept
{}


void PileUpWeight::BeginRun(Dataset const &)
{
    // Save pointer to pile-up reader
    puPlugin = dynamic_cast<PileUpReader const *>(GetDependencyPlugin(puPluginName));
    
    
    // Load the distribution of expected pile-up as generated for the current dataset. (This is)
    //not implemented yet.)
    if (mcPUFile)
    {
        // Check if a MC-truth histogram is available for the new dataset and update mcPUHist if
        //needed. Return on success. Make sure to rescale the histograms with "weight" option
        
        throw std::logic_error("PileUpWeight::BeginRun: Reweighting with actual MC pile-up "
         "distributions described in a file is not implemented yet.");
    }
    
    
    // If the control reaches this point, either file with MC-truth histograms has not been
    //specified or no dataset-specific histogram is available for the current file. Anyway the
    //nominal pile-up distribution should be used
    if (not mcPUHist or std::string(mcPUHist->GetName()) != "nominal")
    {
        // MC distribution for RunIISpring15DR74-Asympt25ns_MCRUN2_74_V9 ("Startup2015") [1-2]
        //[1] https://twiki.cern.ch/twiki/bin/view/CMS/PdmVPileUpDescription?rev=25#Startup2015
        //[2] https://hypernews.cern.ch/HyperNews/CMS/get/physics-validation/2422.html?inline=-1
        std::vector<double> pileUpTruthHist = {4.8551E-07, 1.74806E-06, 3.30868E-06, 1.62972E-05,
          4.95667E-05, 0.000606966, 0.003307249, 0.010340741, 0.022852296, 0.041948781,
          0.058609363, 0.067475755, 0.072817826, 0.075931405, 0.076782504, 0.076202319,
          0.074502547, 0.072355135, 0.069642102, 0.064920999, 0.05725576, 0.047289348,
          0.036528446, 0.026376131, 0.017806872, 0.011249422, 0.006643385, 0.003662904,
          0.001899681, 0.00095614, 0.00050028, 0.000297353, 0.000208717, 0.000165856,
          0.000139974, 0.000120481, 0.000103826, 8.88868E-05, 7.53323E-05, 6.30863E-05,
          5.21356E-05, 4.24754E-05, 3.40876E-05, 2.69282E-05, 2.09267E-05, 1.5989E-05,
          4.8551E-06, 2.42755E-06, 4.8551E-07, 2.42755E-07, 1.21378E-07, 4.8551E-08};
        
        
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


Plugin *PileUpWeight::Clone() const
{
    return new PileUpWeight(*this);
}


PileUpWeight::Weights const &PileUpWeight::GetWeights() const
{
    return weights;
}


bool PileUpWeight::ProcessEvent()
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


void PileUpWeight::ReadTargetDistribution(std::string const &dataPUFileName)
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

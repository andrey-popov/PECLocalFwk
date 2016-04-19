#include <mensura/extensions/PileUpWeight.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/core/PileUpReader.hpp>
#include <mensura/core/Processor.hpp>
#include <mensura/core/ROOTLock.hpp>

#include <TKey.h>

#include <cstdlib>
#include <sstream>
#include <stdexcept>


PileUpWeight::PileUpWeight(std::string const &name, std::string const &dataPUFileName,
  std::string const &mcPUFileName, double systError_):
    EventWeightPlugin(name),
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


PileUpWeight::PileUpWeight(std::string const &dataPUFileName, std::string const &mcPUFileName,
  double systError_):
    EventWeightPlugin("PileUpWeight"),
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


PileUpWeight::~PileUpWeight() noexcept
{}


void PileUpWeight::BeginRun(Dataset const &dataset)
{
    // Save pointer to pile-up reader
    puPlugin = dynamic_cast<PileUpReader const *>(GetDependencyPlugin(puPluginName));
    
    
    // Update the simulated pile-up profile if needed
    std::string const simProfileLabel = dataset.GetSourceDatasetID();
    
    if (not mcPUHist or std::string(mcPUHist->GetName()) != simProfileLabel)
    {
        ROOTLock::Lock();
        
        // Try to read the desired profile from the file
        TList const *fileContent = mcPUFile->GetListOfKeys();
        TKey *key = dynamic_cast<TKey *>(fileContent->FindObject(simProfileLabel.c_str()));
        
        if (key)
        {
            mcPUHist.reset(dynamic_cast<TH1 *>(key->ReadObj()));
            mcPUHist->SetDirectory(nullptr);
        }
        else
        {
            // There is no specific histogram for the current dataset. Use the nominal one
            TH1 *nominalProfile = dynamic_cast<TH1 *>(mcPUFile->Get("nominal"));
            
            if (not nominalProfile)
            {
                std::ostringstream ost;
                ost << "PileUpWeight::BeginRun: File wile pile-up profiles \"" <<
                  mcPUFile->GetName() << "\" does not contain the required histogram \"nominal\".";
                throw std::runtime_error(ost.str());
            }
            
            mcPUHist.reset(nominalProfile);
            mcPUHist->SetDirectory(nullptr);
        }
        
        ROOTLock::Unlock();
        
        
        // Make sure the histogram is normalized
        mcPUHist->Scale(1. / mcPUHist->Integral(0, -1), "width");
    }
    
    
    // Initialize weights
    weights.assign(3, 0.);
}


Plugin *PileUpWeight::Clone() const
{
    return new PileUpWeight(*this);
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
    weights.at(0) = dataPUHist->GetBinContent(bin) / mcProb;
    
    bin = dataPUHist->FindFixBin(nTruth * (1. + systError));
    weights.at(1) = dataPUHist->GetBinContent(bin) / mcProb * (1. + systError);
    //^ The last multiplier is needed to correct for the total normalisation due to rescale in
    //the variable of integration. Same applies to the down weight below
    
    bin = dataPUHist->FindFixBin(nTruth * (1. - systError));
    weights.at(2) = dataPUHist->GetBinContent(bin) / mcProb * (1. - systError);
    
    
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

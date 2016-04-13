#include <mensura/extensions/LeptonSFWeight.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/core/LeptonReader.hpp>
#include <mensura/core/Processor.hpp>
#include <mensura/core/ROOTLock.hpp>

#include <TFile.h>

#include <sstream>
#include <stdexcept>


LeptonSFWeight::LeptonSFWeight(std::string const &name,
  Lepton::Flavour targetFlavour_, std::string const &srcFileName,
  std::list<std::string> const &histogramNames):
    AnalysisPlugin(name),  // need to call constructor of a virtual base class explicitly
    EventWeightPlugin(name),
    leptonPluginName("Leptons"), leptonPlugin(nullptr),
    targetFlavour(targetFlavour_)
{
    LoadScaleFactors(srcFileName, histogramNames);
    
    // The plugin will calculate one weight per event
    EventWeightPlugin::weights.push_back(0.);
}


LeptonSFWeight::LeptonSFWeight(Lepton::Flavour targetFlavour_, std::string const &srcFileName,
  std::list<std::string> const &histogramNames):
    AnalysisPlugin("LeptonSFWeight"),
    EventWeightPlugin("LeptonSFWeight"),
    leptonPluginName("Leptons"), leptonPlugin(nullptr),
    targetFlavour(targetFlavour_)
{
    LoadScaleFactors(srcFileName, histogramNames);
    
    // The plugin will calculate one weight per event
    EventWeightPlugin::weights.push_back(0.);
}


LeptonSFWeight::~LeptonSFWeight() noexcept
{}


void LeptonSFWeight::BeginRun(Dataset const &)
{
    // Save pointers to the plugin that provides leptons
    leptonPlugin = dynamic_cast<LeptonReader const *>(GetDependencyPlugin(leptonPluginName));
}


Plugin *LeptonSFWeight::Clone() const
{
    return new LeptonSFWeight(*this);
}


void LeptonSFWeight::LoadScaleFactors(std::string const &srcFileName,
  std::list<std::string> const &histogramNames)
{
    // Open the source file and read the histograms with scale factors from it
    ROOTLock::Lock();
    
    FileInPath pathBuilder;
    std::string const srcFilePath(pathBuilder.Resolve("LeptonSF", srcFileName));
    std::unique_ptr<TFile> srcFile(TFile::Open(srcFilePath.c_str()));
    
    for (std::string const &histName: histogramNames)
    {
        TH2D *hist = dynamic_cast<TH2D *>(srcFile->Get(histName.c_str()));
        
        if (not hist)
        {
            std::ostringstream message;
            message << "LeptonSFWeight::LeptonSFWeight: Cannot find histogram \"" <<
              histName << "\" in file \"" << srcFilePath << "\".";
            
            throw std::runtime_error(message.str());
        }
        
        hist->SetDirectory(nullptr);
        sfComponents.emplace_back(hist);
    }
    
    srcFile.reset();
    
    ROOTLock::Unlock();
}


bool LeptonSFWeight::ProcessEvent()
{
    // A shortcut for tight leptons
    auto const &leptons = leptonPlugin->GetLeptons();
    
    
    // Loop over the leptons and calculate the total scale factor
    double scaleFactor = 1.;
    
    for (auto const &lepton: leptons)
    {
        // Skip leptons of wrong flavour
        if (lepton.GetFlavour() != targetFlavour)
            continue;
        
        // Loop over components of the scale factor
        for (auto const &hist: sfComponents)
        {
            int const bin = hist->FindFixBin(lepton.Pt(), lepton.Eta());
            scaleFactor *= hist->GetBinContent(bin);
        }
    }
    
    
    // Update the nominal weight
    weights.at(0) = scaleFactor;
    
    
    return true;
}

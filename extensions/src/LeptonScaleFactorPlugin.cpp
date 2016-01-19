#include <LeptonScaleFactorPlugin.hpp>

#include <ROOTLock.hpp>
#include <FileInPath.hpp>
#include <Processor.hpp>

#include <stdexcept>
#include <sstream>


using namespace std;


LeptonScaleFactorPlugin::LeptonScaleFactorPlugin(string const &name_,
 Lepton::Flavour targetFlavour_, string const &srcFileName, list<string> histogramNames):
    AnalysisPlugin(name_),  // need to call constructor of a virtual base class explicitly
    EventWeightPlugin(name_),
    targetFlavour(targetFlavour_)
{
    // Open the source file and read the histograms with scale factors from it
    ROOTLock::Lock();
    
    FileInPath pathBuilder;
    unique_ptr<TFile> srcFile(TFile::Open(pathBuilder.Resolve("LeptonSF", srcFileName).c_str()));
    
    for (string const &histName: histogramNames)
    {
        TH2D *hist = dynamic_cast<TH2D *>(srcFile->Get(histName.c_str()));
        
        if (not hist)
        {
            ostringstream message;
            message << "LeptonScaleFactorPlugin::LeptonScaleFactorPlugin: Cannot find " <<
             "histogram \"" << histName << "\" in file \"" <<
             pathBuilder.Resolve("LeptonSF", srcFileName) << "\".";
            
            throw runtime_error(message.str());
        }
        
        hist->SetDirectory(nullptr);
        sfComponents.emplace_back(hist);
    }
    
    srcFile.reset();
    
    ROOTLock::Unlock();
    
    
    // The plugin will calculate one weight per event
    EventWeightPlugin::weights.push_back(0.);
}


LeptonScaleFactorPlugin::LeptonScaleFactorPlugin(LeptonScaleFactorPlugin const &src):
    AnalysisPlugin(src),  // need to call constructor of a virtual base class explicitly
    EventWeightPlugin(src),
    targetFlavour(src.targetFlavour),
    sfComponents(src.sfComponents)
{}


LeptonScaleFactorPlugin::~LeptonScaleFactorPlugin() noexcept
{}


Plugin *LeptonScaleFactorPlugin::Clone() const
{
    return new LeptonScaleFactorPlugin(*this);
}


void LeptonScaleFactorPlugin::BeginRun(Dataset const &)
{
    // Save the pointer to the reader plugin
    reader = dynamic_cast<PECReaderPlugin const *>(master->GetPluginBefore("Reader", name));
}


bool LeptonScaleFactorPlugin::ProcessEvent()
{
    // A shortcut for tight leptons
    auto const &leptons = (*reader)->GetLeptons();
    
    
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

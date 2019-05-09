#include <mensura/LeptonSFWeight.hpp>

#include <mensura/FileInPath.hpp>
#include <mensura/LeptonReader.hpp>
#include <mensura/Processor.hpp>
#include <mensura/ROOTLock.hpp>

#include <TFile.h>
#include <TObjString.h>

#include <boost/algorithm/string.hpp>

#include <cmath>
#include <sstream>
#include <stdexcept>


// An auxiliary function to construct functions to evaluate lepton parameters based on their
//names
std::function<double (Lepton const &)> BuildParamExpression(std::string paramName)
{
    if (paramName == "pt")
        return [](Lepton const &l){return l.Pt();};
    else if (paramName == "eta")
        return [](Lepton const &l){return l.Eta();};
    else if (paramName == "etaSC")
        return [](Lepton const &l){return l.UserFloat("etaSC");};
    else if (paramName == "absEta")
        return [](Lepton const &l){return std::abs(l.Eta());};
    else if (paramName == "absEtaSC")
        return [](Lepton const &l){return std::abs(l.UserFloat("etaSC"));};
    else
        throw std::invalid_argument(paramName);
}


LeptonSFWeight::LeptonSFWeight(std::string const &name,
  Lepton::Flavour targetFlavour_, std::string const &srcFileName,
  std::list<std::string> const &histogramNames):
    EventWeightPlugin(name),
    leptonPluginName("Leptons"), leptonPlugin(nullptr),
    targetFlavour(targetFlavour_)
{
    LoadScaleFactors(srcFileName, histogramNames);
    
    // The plugin will calculate one weight per event with one systematic variation
    EventWeightPlugin::weights = {0., 0., 0.};
}


LeptonSFWeight::LeptonSFWeight(Lepton::Flavour targetFlavour_, std::string const &srcFileName,
  std::list<std::string> const &histogramNames):
    EventWeightPlugin("LeptonSFWeight"),
    leptonPluginName("Leptons"), leptonPlugin(nullptr),
    targetFlavour(targetFlavour_)
{
    LoadScaleFactors(srcFileName, histogramNames);
    
    // The plugin will calculate one weight per event with one systematic variation
    EventWeightPlugin::weights = {0., 0., 0.};
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
    
    std::string const srcFilePath(FileInPath::Resolve("LeptonSF", srcFileName));
    std::unique_ptr<TFile> srcFile(TFile::Open(srcFilePath.c_str()));
    
    for (std::string const &histName: histogramNames)
    {
        HistAdjustableParams sfObject;
        
        // Read the histogram with scale factors
        TH2 *hist = dynamic_cast<TH2 *>(srcFile->Get(histName.c_str()));
        
        if (not hist)
        {
            std::ostringstream message;
            message << "LeptonSFWeight::LoadScaleFactors: Cannot find histogram \"" <<
              histName << "\" in file \"" << srcFilePath << "\".";
            
            throw std::runtime_error(message.str());
        }
        
        hist->SetDirectory(nullptr);
        sfObject.hist.reset(hist);
        
        
        // Set up parameters of the histogram
        TObjString *paramNamesRawStr =
          dynamic_cast<TObjString *>(srcFile->Get((histName + "__params").c_str()));
        
        if (not paramNamesRawStr)
        {
            // No string describing parameters of the histogram is provided. Use default ones
            sfObject.x = [](Lepton const &l){return l.Pt();};
            sfObject.y = [](Lepton const &l){return l.Eta();};
        }
        else
        {
            // Parse the string with parameter names
            std::string const paramNamesRaw(paramNamesRawStr->GetString().Data());
            std::vector<std::string> paramNames;
            boost::split(paramNames, paramNamesRaw, boost::is_any_of(";"));
            
            if (paramNames.size() != 2)
            {
                std::ostringstream message;
                message << "LeptonSFWeight::LoadScaleFactors: Parameter string \"" <<
                  histName << "__params\" in file \"" << srcFilePath << "\" has value \"" <<
                  paramNamesRaw << "\" and defines " << paramNames.size() <<
                  " parameters, while it must define exactly 2.";
                throw std::runtime_error(message.str());
            }
            
            
            // Set expressions to evaluate inputs for the histogram
            try
            {
                sfObject.x = BuildParamExpression(paramNames[0]);
                sfObject.y = BuildParamExpression(paramNames[1]);
            }
            catch (std::invalid_argument const &excp)
            {
                std::ostringstream message;
                message << "LeptonSFWeight::LoadScaleFactors: Unrecognized parameter \"" <<
                  excp.what() << "\" found in parameter string \"" << histName <<
                  "__params\" in file \"" << srcFilePath <<
                  "\". The value of the parameter string is \"" << paramNamesRaw << "\".";
                throw std::runtime_error(message.str());
            }
        }
        
        
        sfComponents.emplace_back(std::move(sfObject));
    }
    
    srcFile.reset();
    
    ROOTLock::Unlock();
}


bool LeptonSFWeight::ProcessEvent()
{
    // A shortcut for tight leptons
    auto const &leptons = leptonPlugin->GetLeptons();
    
    
    // Loop over the leptons and calculate the total scale factor and its relative uncertainty.
    //Relative uncertainty of a product of multiple scale factors for, potentially, multiple
    //leptons is simply a sum of all individual relative uncertainties in quadrature.
    double scaleFactor = 1.;
    double relUnc2 = 0.;
    
    for (auto const &lepton: leptons)
    {
        // Skip leptons of wrong flavour
        if (lepton.GetFlavour() != targetFlavour)
            continue;
        
        
        // Loop over components of the scale factor
        for (auto const &sfObject: sfComponents)
        {
            int const bin = sfObject.hist->FindFixBin(sfObject.x(lepton), sfObject.y(lepton));
            double const sf = sfObject.hist->GetBinContent(bin);
            scaleFactor *= sf;
            relUnc2 += std::pow(sfObject.hist->GetBinError(bin) / sf, 2);
        }
    }
    
    
    // Update the nominal weight
    weights.at(0) = scaleFactor;
    
    double const unc = scaleFactor * std::sqrt(relUnc2);
    weights.at(1) = scaleFactor + unc;
    weights.at(2) = scaleFactor - unc;
    
    
    return true;
}

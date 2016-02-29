#include <PECFwk/extensions/BTagEffService.hpp>

#include <PECFwk/core/FileInPath.hpp>
#include <PECFwk/core/PhysicsObjects.hpp>
#include <PECFwk/core/ROOTLock.hpp>

#include <algorithm>
#include <sstream>
#include <stdexcept>


BTagEffService::BTagEffService(std::string const &name, std::string const &fileName,
  std::string const &directory):
    Service(name),
    inFileDirectory(directory)
{
    OpenInputFile(fileName);
}


BTagEffService::BTagEffService(std::string const &fileName, std::string const &directory /*= ""*/):
    Service("BTagEff"),
    inFileDirectory(directory)
{
    OpenInputFile(fileName);
}


BTagEffService::~BTagEffService() noexcept
{}


BTagEffService::BTagEffService(BTagEffService const &src) noexcept:
    Service(src),
    srcFile(src.srcFile),  // shared
    inFileDirectory(src.inFileDirectory),
    processLabelMap(src.processLabelMap),
    defaultProcessLabel(src.defaultProcessLabel)
{}


void BTagEffService::BeginRun(Dataset const &dataset)
{
    // Find the label corresponding to the process in the dataset
    std::string newProcessLabel;
    auto const mapRuleIt = std::find_if(processLabelMap.begin(), processLabelMap.end(),
      [&dataset](decltype(*processLabelMap.cbegin()) &rule){
        return dataset.TestProcess(rule.first);});
    
    if (mapRuleIt != processLabelMap.end())
        newProcessLabel = mapRuleIt->second;
    else
    {
        // No label is defined for this dataset. Check if the default one is available
        if (defaultProcessLabel.length() == 0)
        {
            std::ostringstream ost;
            ost << "BTagEffService::BeginRun: Cannot find which histogram with b-tagging " <<
              "efficiencies should be used for the dataset containing file \"" <<
              dataset.GetFiles().front().GetBaseName() << ".root\". " <<
              "No rule to define the histogram name is available for the dataset, and no " <<
              "default name is specified.";
            
            throw std::runtime_error(ost.str());
        }
        
        // Apparently, there is a default label. Use it
        newProcessLabel = defaultProcessLabel;
    }
    
    
    // Clear the map with efficiency histograms if outdated
    if (newProcessLabel != curProcessLabel)
    {
        effHists.clear();
        curProcessLabel = newProcessLabel;
    }
}


Service *BTagEffService::Clone() const
{
    return new BTagEffService(*this);
}


double BTagEffService::GetEfficiency(BTagger const &bTagger, double pt, double eta,
  unsigned flavour) const
{
    // Find the appropriate efficiency histogram. Load it if needed
    auto histGroupIt = effHists.find(bTagger);
    TH2 *hist = nullptr;
    
    if (histGroupIt == effHists.end())
    {
        // Try to load histograms for the given b-tagger
        const_cast<BTagEffService *>(this)->LoadEfficiencies(bTagger);
        hist = effHists.at(bTagger).at(flavour).get();
    }
    else
        hist = histGroupIt->second.at(flavour).get();
    
    
    // Make sure the histogram exists
    if (not hist)
    {
        std::ostringstream ost;
        ost << "BTagEffService::GetEfficiency: Failed to find an efficiency histogram for " <<
          "b tagger " << bTagger.GetTextCode() << ", process label \"" << curProcessLabel <<
          "\", jet flavour " << flavour << ".";
        throw std::runtime_error(ost.str());
    }

    
    // Return the efficiency
    return hist->GetBinContent(hist->FindFixBin(pt, eta));
}


double BTagEffService::GetEfficiency(BTagger const &bTagger, Jet const &jet)  const
{
    return GetEfficiency(bTagger, jet.Pt(), jet.Eta(), jet.GetParentID());
}


void BTagEffService::SetDefaultProcessLabel(std::string const &label)
{
    defaultProcessLabel = label;
}


void BTagEffService::SetProcessLabel(Dataset::Process code, std::string const &label)
{
    // Check if the given process code has already been registered
    auto const mapRuleIt = std::find_if(processLabelMap.begin(), processLabelMap.end(),
     [code](decltype(*processLabelMap.cbegin()) &rule){return (rule.first == code);});
    
    
    if (mapRuleIt == processLabelMap.end())
    {
        // Add a new mapping rule
        processLabelMap.emplace_back(code, label);
    }
    else
    {
        // Reset the label registered for this process code
        mapRuleIt->second = label;
    }
}


void BTagEffService::SetProcessLabel(std::list<Dataset::Process> const &codes,
  std::string const &label)
{
    for (auto const &code: codes)
        SetProcessLabel(code, label);
}


void BTagEffService::OpenInputFile(std::string const &fileName)
{
    // Guard creation of a ROOT file
    ROOTLock::Lock();
    
    // Resolve path to the source file and open it. If the file is missing, pathBuilder will throw
    //an exception
    FileInPath pathBuilder;
    TFile *fp = TFile::Open(pathBuilder.Resolve("BTag", fileName).c_str());
    srcFile.reset(fp);
    
    ROOTLock::Unlock();
    
    
    // Make sure the in-file directory path is either empty or terminates with a slash
    if (inFileDirectory.length() > 0 and inFileDirectory[inFileDirectory.length() - 1] != '/')
        inFileDirectory += '/';
}


void BTagEffService::LoadEfficiencies(BTagger const &bTagger)
{
    using namespace std;
    
    string const bTaggerCode(bTagger.GetTextCode());
    
    
    // Read histograms for all jet flavours. This is not a thread-safe operation
    ROOTLock::Lock();
    
    shared_ptr<TH2> bHist(dynamic_cast<TH2 *>(srcFile->Get(
      (inFileDirectory + bTaggerCode + "/" + curProcessLabel + "_b").c_str())));
    shared_ptr<TH2> cHist(dynamic_cast<TH2 *>(srcFile->Get(
      (inFileDirectory + bTaggerCode + "/" + curProcessLabel + "_c").c_str())));
    shared_ptr<TH2> udsgHist(dynamic_cast<TH2 *>(srcFile->Get(
      (inFileDirectory + bTaggerCode + "/" + curProcessLabel + "_udsg").c_str())));
    
    // Make sure the histograms are not associated with a file
    for (auto const &p: {bHist, cHist, udsgHist})
    {
        if (p)
            p->SetDirectory(nullptr);
    }
    
    ROOTLock::Unlock();
    
    
    // Make sure at least some histograms with efficiencies have been read from the file
    if (not bHist and not cHist and not udsgHist)
        throw runtime_error("BTagEffService::LoadEfficiencies: No histograms for b tagger "s +
          bTaggerCode + " are present in the data file.");
    
    
    // Add the histograms to the histogram map if the pointers are not null
    auto &histMap = effHists[bTagger];
    histMap.clear();
    
    if (bHist)
        histMap[5] = bHist;
    
    if (cHist)
        histMap[4] = cHist;
    
    if (udsgHist)
    {
        histMap[0] = udsgHist;
        histMap[1] = udsgHist;
        histMap[2] = udsgHist;
        histMap[3] = udsgHist;
        histMap[21] = udsgHist;
    }
}

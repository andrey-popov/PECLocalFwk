#include <mensura/extensions/BTagEffService.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/core/PhysicsObjects.hpp>
#include <mensura/core/ROOTLock.hpp>

#include <algorithm>
#include <sstream>
#include <stdexcept>

/**/#include <iostream>


BTagEffService::BTagEffService(std::string const &name, std::string const &path):
    Service(name)
{
    OpenInputFile(path);
}


BTagEffService::BTagEffService(std::string const &path):
    Service("BTagEff")
{
    OpenInputFile(path);
}


BTagEffService::BTagEffService(BTagEffService const &src) noexcept:
    Service(src),
    srcFile(src.srcFile),  // shared
    inFileDirectory(src.inFileDirectory),
    effLabelRules(src.effLabelRules),
    defaultEffLabel(src.defaultEffLabel)
{}


void BTagEffService::BeginRun(Dataset const &dataset)
{
    // Find the efficiency label corresponding to the new dataset
    std::string newEffLabel;
    auto const &datasetId = dataset.GetSourceDatasetID();
    std::smatch matchResults;
    
    for (auto const &rule: effLabelRules)
    {
        if (std::regex_match(datasetId, matchResults, rule.first))
        {
            newEffLabel = rule.second;
            break;
        }
    }
    
    
    if (matchResults.empty())
    {
        // No label is defined for this dataset. Check if the default one is available
        if (defaultEffLabel.empty())
        {
            std::ostringstream message;
            message << "BTagEffService[\"" << GetName() << "\"]::BeginRun: " <<
              "Cannot determine efficiency label for datataset with ID \"" << datasetId <<
              "\". No rule is satisfied, and no default label has been given.";
            throw std::runtime_error(message.str());
        }
        
        // Apparently, there is a default label. Use it
        newEffLabel = defaultEffLabel;
    }
    
    
    // Clear the map with efficiency histograms if outdated
    if (newEffLabel != curEffLabel)
    {
        effHists.clear();
        curEffLabel = newEffLabel;
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
        std::ostringstream message;
        message << "BTagEffService[\"" << GetName() << "\"]::GetEfficiency: " <<
          "Failed to find an efficiency histogram for b tagger " << bTagger.GetTextCode() <<
          ", efficiency label \"" << curEffLabel << "\", jet flavour " << flavour << ".";
        throw std::runtime_error(message.str());
    }

    
    // Return the efficiency
    return hist->GetBinContent(hist->FindFixBin(pt, eta));
}


double BTagEffService::GetEfficiency(BTagger const &bTagger, Jet const &jet)  const
{
    return GetEfficiency(bTagger, jet.Pt(), jet.Eta(), jet.Flavour(Jet::FlavourType::Hadron));
}


void BTagEffService::SetEffLabel(std::string const &datasetIdMask, std::string const &label)
{
    try
    {
        effLabelRules.emplace_back(std::regex(datasetIdMask), label);
    }
    catch (std::regex_error const &)
    {
        std::ostringstream message;
        message << "BTagEffService[\"" << GetName() << "\"]::SetEffLabel: " <<
          "Failed to construct a regular expression from mask \"" << datasetIdMask << "\".";
        throw std::runtime_error(message.str());
    }
}


void BTagEffService::SetEffLabel(
  std::initializer_list<std::pair<std::string, std::string>> const &rules)
{
    for (auto const &rule: rules)
        SetEffLabel(rule.first, rule.second);
}


void BTagEffService::SetDefaultEffLabel(std::string const &label)
{
    defaultEffLabel = label;
}


void BTagEffService::LoadEfficiencies(BTagger const &bTagger)
{
    using namespace std;
    
    string const bTaggerCode(bTagger.GetTextCode());
    
    
    // Read histograms for all jet flavours. This is not a thread-safe operation
    ROOTLock::Lock();
    
    shared_ptr<TH2> bHist(dynamic_cast<TH2 *>(srcFile->Get(
      (inFileDirectory + bTaggerCode + "/" + curEffLabel + "_b").c_str())));
    shared_ptr<TH2> cHist(dynamic_cast<TH2 *>(srcFile->Get(
      (inFileDirectory + bTaggerCode + "/" + curEffLabel + "_c").c_str())));
    shared_ptr<TH2> udsgHist(dynamic_cast<TH2 *>(srcFile->Get(
      (inFileDirectory + bTaggerCode + "/" + curEffLabel + "_udsg").c_str())));
    
    // Make sure the histograms are not associated with a file
    for (auto const &p: {bHist, cHist, udsgHist})
    {
        if (p)
            p->SetDirectory(nullptr);
    }
    
    ROOTLock::Unlock();
    
    
    // Make sure at least some histograms with efficiencies have been read from the file
    if (not bHist and not cHist and not udsgHist)
    {
        ostringstream message;
        message << "BTagEffService[\"" << GetName() << "\"]::LoadEfficiencies: " <<
          "No histogram for b tagger \"" << bTaggerCode << "\" with efficiency label \"" <<
          curEffLabel << "\" is present in the data file.";
        throw runtime_error(message.str());
    }
    
    
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


void BTagEffService::OpenInputFile(std::string const &path)
{
    // Split the given path into file path and in-file directory
    unsigned const nSep = std::count(path.begin(), path.end(), ':');
    std::string filePath;
    
    if (nSep == 0)
    {
        filePath = path;
        inFileDirectory = "";
    }
    else if (nSep == 1)
    {
        auto const pos = path.find(":");
        filePath = path.substr(0, pos);
        inFileDirectory = path.substr(pos + 1);
        
        // Make sure the in-file directory terminates with a slash
        if (inFileDirectory != "" and inFileDirectory[inFileDirectory.length() - 1] != '/')
            inFileDirectory += '/';
    }
    else
    {
        std::ostringstream message;
        message << "BTagEffService[\"" << GetName() << "\"]::OpenInputFile: " <<
          "Path \"" << path << "\" contains too many colons.";
        throw std::runtime_error(message.str());
    }
    
    
    // Guard creation of a ROOT file
    ROOTLock::Lock();
    
    // Resolve path to the input file and open it. If the file is missing, FileInPath will throw
    //an exception
    TFile *fp = TFile::Open(FileInPath::Resolve("BTag", filePath).c_str());
    srcFile.reset(fp);
    
    ROOTLock::Unlock();
}

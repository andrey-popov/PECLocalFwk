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


BTagEffService::BTagEffService(BTagEffService const &src):
    Service(src),
    srcFile(src.srcFile),   // shared
    inFileDirectory(src.inFileDirectory),
    processLabelMap(src.processLabelMap),
    defaultProcessLabel(src.defaultProcessLabel),
    effHists(src.effHists)  // histograms are shared
{}


BTagEffService::~BTagEffService() noexcept
{}


Service *BTagEffService::Clone() const
{
    return new BTagEffService(*this);
}


void BTagEffService::BeginRun(Dataset const &dataset)
{
    // Clear the map with efficiency histograms
    effHists.clear();
    
    
    // Find the label corresponding to the process in the dataset
    std::string curProcessLabel;
    auto const mapRuleIt = std::find_if(processLabelMap.begin(), processLabelMap.end(),
      [&dataset](decltype(*processLabelMap.cbegin()) &rule){
        return dataset.TestProcess(rule.first);});
    
    if (mapRuleIt != processLabelMap.end())
        curProcessLabel = mapRuleIt->second;
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
        curProcessLabel = defaultProcessLabel;
    }
    
    
    // Loop over possible working points
    for (auto const &wp: {BTagger::WorkingPoint::Tight, BTagger::WorkingPoint::Medium,
     BTagger::WorkingPoint::Loose})
    {
        std::string const wpCode(BTagger::WorkingPointToTextCode(wp));
        
        
        // Read histograms for all jet flavours. This is not a thread-safe operation
        ROOTLock::Lock();
        
        std::shared_ptr<TH2> bHist(dynamic_cast<TH2 *>(srcFile->Get(
         (inFileDirectory + curProcessLabel + "_b_" + wpCode).c_str())));
        std::shared_ptr<TH2> cHist(dynamic_cast<TH2 *>(srcFile->Get(
         (inFileDirectory + curProcessLabel + "_c_" + wpCode).c_str())));
        std::shared_ptr<TH2> udsHist(dynamic_cast<TH2 *>(srcFile->Get(
         (inFileDirectory + curProcessLabel + "_uds_" + wpCode).c_str())));
        std::shared_ptr<TH2> gHist(dynamic_cast<TH2 *>(srcFile->Get(
         (inFileDirectory + curProcessLabel + "_g_" + wpCode).c_str())));
        
        // Make sure the histograms are not associated with a file
        for (auto const &p: {bHist, cHist, udsHist, gHist})
        {
            if (p)
                p->SetDirectory(nullptr);
        }
        
        ROOTLock::Unlock();
        
        
        // Add the histograms to the histogram map if the pointers are not null
        if (bHist)
            effHists[std::make_pair(wp, 5)] = bHist;
        
        if (cHist)
            effHists[std::make_pair(wp, 4)] = cHist;
        
        if (udsHist)
        {
            effHists[std::make_pair(wp, 1)] = udsHist;
            effHists[std::make_pair(wp, 2)] = udsHist;
            effHists[std::make_pair(wp, 3)] = udsHist;
        }
        
        if (gHist)
        {
            effHists[std::make_pair(wp, 21)] = gHist;
            effHists[std::make_pair(wp, 0)] = gHist;
            //^ Jets with unidentified flavour are considered along with gluons. The motivation is
            //that pile-up jets usually obtain a flavour of either 0 or 21
        }
    }
}


double BTagEffService::GetEfficiency(BTagger::WorkingPoint wp, double pt, double eta, int flavour)
  const
{
    // Find the appropriate efficiency histogram
    auto histIt = effHists.find(std::make_pair(wp, abs(flavour)));
    
    if (histIt == effHists.end())
    {
        std::ostringstream ost;
        ost << "BTagEffService::GetEfficiency: Failed to find an efficiency histogram for " <<
          "working point " << BTagger::WorkingPointToTextCode(wp) << " and jet flavour " <<
          flavour << ".";
        
        throw std::runtime_error(ost.str());
    }
    
    
    // Find bin that contains the jet
    int const bin = histIt->second->FindFixBin(pt, eta);
    
        
    return histIt->second->GetBinContent(bin);
}


double BTagEffService::GetEfficiency(BTagger::WorkingPoint wp, Jet const &jet)  const
{
    return GetEfficiency(wp, jet.Pt(), jet.Eta(), jet.GetParentID());
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

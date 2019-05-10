#include <mensura/PECReader/PECTriggerObjectReader.hpp>

#include <mensura/Processor.hpp>
#include <mensura/ROOTLock.hpp>
#include <mensura/PECReader/PECInputData.hpp>

#include "Candidate.hpp"

#include <TTree.h>

#include <sstream>
#include <stdexcept>


PECTriggerObjectReader::PECTriggerObjectReader(std::string const &name,
  std::initializer_list<std::string> triggerMasks_ /*= {".*"}*/):
    ReaderPlugin(name),
    inputDataPluginName("InputData"), inputDataPlugin(nullptr),
    treeName("pecTriggerObjects/TriggerObjects")
{
    // Build trigger masks
    triggerMasks.reserve(triggerMasks_.size());
    
    for (auto const &mask: triggerMasks_)
        triggerMasks.emplace_back(mask);
}


PECTriggerObjectReader::PECTriggerObjectReader(
  std::initializer_list<std::string> triggerMasks_ /*= {".*"}*/):
    PECTriggerObjectReader("TriggerObjects", triggerMasks_)
{}


void PECTriggerObjectReader::BeginRun(Dataset const &)
{
    // Save pointer to the plugin that provides access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(GetDependencyPlugin(inputDataPluginName));
    
    
    // Set up reading of the tree with trigger objects
    inputDataPlugin->LoadTree(treeName);
    
    ROOTLock::Lock();
    TTree *t = inputDataPlugin->ExposeTree(treeName);
    
    
    // Find out what branches should be read. Their names are put into the trigger index map.
    TObjArray const &branches = *t->GetListOfBranches();
    triggerIndexMap.clear();
    unsigned index = 0;
    
    for (unsigned iBranch = 0; iBranch < unsigned(branches.GetEntries()); ++iBranch)
    {
        std::string const branchName(branches[iBranch]->GetName());
        bool matched = false;
        
        for (auto const &mask: triggerMasks)
            if (std::regex_match(branchName, mask))
            {
                matched = true;
                break;
            }
        
        if (matched)
        {
            triggerIndexMap[branchName] = index;
            ++index;
        }
    }
    
    
    if (triggerIndexMap.size() == 0)
    {
        std::ostringstream message;
        message << "PECTriggerObjectReader[\"" << GetName() << "\"]::BeginRun: "
          "None of stored trigger filters match provided mask.";
        throw std::runtime_error(message.str());
    }
    
    
    // Set buffers
    buffers.resize(triggerIndexMap.size());
    bufferPointers.resize(triggerIndexMap.size());
    
    t->SetBranchStatus("*", false);
    
    for (auto const &p: triggerIndexMap)
    {
        bufferPointers.at(p.second) = &buffers.at(p.second);
        t->SetBranchStatus((p.first + ".*").c_str(), true);
        t->SetBranchAddress(p.first.c_str(), &bufferPointers.at(p.second));
    }
    
    ROOTLock::Unlock();
    
    
    triggerObjects.resize(triggerIndexMap.size());
}


Plugin *PECTriggerObjectReader::Clone() const
{
    return new PECTriggerObjectReader(*this);
}


unsigned PECTriggerObjectReader::GetFilterIndex(std::string const &triggerFilterName) const
{
    auto const res = triggerIndexMap.find(triggerFilterName);
    
    if (res == triggerIndexMap.end())
    {
        std::ostringstream message;
        message << "PECTriggerObjectReader[\"" << GetName() << "\"]::GetFilterIndex: "
          "Trigger filter \"" << triggerFilterName << "\" has not been booked.";
        throw std::runtime_error(message.str());
    }
    
    return res->second;
}


std::vector<Candidate> const &PECTriggerObjectReader::GetObjects(
  std::string const &triggerFilterName) const
{
    return GetObjects(GetFilterIndex(triggerFilterName));
}


std::vector<Candidate> const &PECTriggerObjectReader::GetObjects(unsigned filterIndex) const
{
    if (filterIndex >= triggerObjects.size())
    {
        std::ostringstream message;
        message << "PECTriggerObjectReader[\"" << GetName() << "\"]::GetObjects: "
          "Given index " << filterIndex << " is out out range.";
        throw std::runtime_error(message.str());
    }
    
    return triggerObjects[filterIndex];
}


bool PECTriggerObjectReader::ProcessEvent()
{
    inputDataPlugin->ReadEventFromTree(treeName);
    
    
    for (unsigned iTrigger = 0; iTrigger < buffers.size(); ++iTrigger)
    {
        auto &curTriggerObjects = triggerObjects[iTrigger];
        
        // Clear from previous event
        curTriggerObjects.clear();
        
        
        // Translate PEC classes into standard representation
        for (auto const &srcObject: buffers[iTrigger])
        {
            Candidate translated;
            translated.SetPtEtaPhiM(srcObject.Pt(), srcObject.Eta(), srcObject.Phi(),
              srcObject.M());
            curTriggerObjects.emplace_back(translated);
        }
    }
    
    
    // Always return true since this reader does not know when the input file is exhausted
    return true;
}

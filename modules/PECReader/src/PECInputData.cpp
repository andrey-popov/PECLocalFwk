#include <PECFwk/PECReader/PECInputData.hpp>

#include <PECFwk/core/Logger.hpp>
#include <PECFwk/core/ROOTLock.hpp>

#include <algorithm>
#include <iterator>
#include <sstream>
#include <stdexcept>


using namespace logging;
using namespace std::string_literals;


PECInputData::PECInputData(std::string const name):
    ReaderPlugin(name),
    nextFileIt(inputFiles.end()),
    eventIDTreeName("pecEventID/EventID"),
    nEvents(0), nextEvent(0),
    eventIDTree(nullptr)
{}


PECInputData::~PECInputData()
{}


void PECInputData::BeginRun(Dataset const &dataset)
{
    // Copy information about files in the dataset and set up the iterator
    auto const &srcFiles = dataset.GetFiles();
    // std::copy(srcFiles.begin(), srcFiles.end(), std::back_inserter(inputFiles));
    inputFiles.emplace_back(srcFiles.front());
    nextFileIt = inputFiles.begin();
    
    // Currently the class only supports datasets with a single file (see documentation for the
    //class). This is why only first file is copied to the list of input files. Print a warning
    //about this
    if (srcFiles.size() > 1)
        logger << "Error in PECInputData: Currently the class only supports datasets " <<
         "containing a single file, but the given dataset consists of " << srcFiles.size() <<
         " files. Only first file (\"" << nextFileIt->name << "\") will be processed." << eom;
}


Plugin *PECInputData::Clone() const
{
    return new PECInputData(GetName());
}


TTree *PECInputData::ExposeTree(std::string const &name) const
{
    auto const res = loadedTrees.find(name);
    
    if (res == loadedTrees.end())
        throw std::logic_error("PECInputData::ExposeTree: Method is called for tree \""s +
          name + "\", which has not been loaded.");
    
    return res->second.get();
}


EventID const &PECInputData::GetEventID() const
{
    return eventID;
}


PECInputData::LoadTreeStatus PECInputData::LoadTree(std::string const &name) const
{
    // Make sure the tree has not been loaded already
    auto const res = loadedTrees.find(name);
    
    if (res != loadedTrees.end())
    {
        logger << "Error in PECInputData::LoadTree: Plugin \"" << GetName() <<
          "\" is requested to load tree \"" << name << "\", which has already been loaded by " <<
          "another plugin. If multiple plugins attempt to read the same tree, this will result " <<
          "in an undefined behaviour." << eom;
        return LoadTreeStatus::AlreadyLoaded;
    }
    
    
    // Read the tree from the input file
    ROOTLock::Lock();
    TTree *tree = dynamic_cast<TTree *>(curInputFile->Get(name.c_str()));
    ROOTLock::Unlock();
    
    if (tree)
        loadedTrees[name] = std::unique_ptr<TTree>(tree);
    else
        return LoadTreeStatus::NotFound;
    
    
    // Starting from the second loaded tree, make sure that the tree contains the same number of
    //events as the first tree (event ID).
    if (loadedTrees.size() > 1 and (unsigned long) tree->GetEntries() != nEvents)
    {
        std::ostringstream message;
        message << "PECInputData::LoadTree: Plugin \"" << GetName() << "\" is requested to " <<
          "load tree \"" << name << "\", which contains a different number of events than " <<
          "preloaded tree \"" << eventIDTreeName << "\" (" << tree->GetEntries() << " vs " <<
          nEvents << ").";
        throw std::runtime_error(message.str());
    }
    
    return LoadTreeStatus::Success;
}


void PECInputData::ReadEventFromTree(std::string const &name) const
{
    auto const res = loadedTrees.find(name);
    
    if (res == loadedTrees.end())
        throw std::logic_error("PECInputData::ReadEventFromTree: Method is called for tree \""s +
          name + "\", which has not been loaded.");
    else
        res->second->GetEntry(nextEvent - 1);
}


bool PECInputData::NextInputFile()
{
    // Delete loaded trees and the current input file
    ROOTLock::Lock();
    
    for (auto &p: loadedTrees)
        p.second.reset();
    
    curInputFile.reset();
    ROOTLock::Unlock();
    
    
    // Check if there are files left in the dataset
    if (nextFileIt == inputFiles.end())
        return false;
    
    
    // Open the new file and read the tree with event IDs
    ROOTLock::Lock();
    
    curInputFile.reset(TFile::Open(nextFileIt->name.c_str()));
    
    if (not curInputFile or curInputFile->IsZombie())
        throw std::runtime_error("PECInputData::NextInputFile: File \""s + nextFileIt->name +
         "\" does not exist or is not a valid ROOT file.");
    
    LoadTree(eventIDTreeName);
    eventIDTree = ExposeTree(eventIDTreeName);
    
    if (not eventIDTree)
        throw std::runtime_error("PECInputData::NextInputFile: File \""s + nextFileIt->name +
         "\" does not contain tree \"pecEventID/EventID\".");
    
    ROOTLock::Unlock();
    
    
    // Update the file iterator for subsequent calls
    ++nextFileIt;
    
    
    // Set up the tree and update counters
    bfEventIDPointer = &bfEventID;
    eventIDTree->SetBranchAddress("eventId", &bfEventIDPointer);
    
    nEvents = eventIDTree->GetEntries();
    nextEvent = 0;
    
    
    return true;
}


bool PECInputData::ProcessEvent()
{
    // Make sure there are events left in the tree and open the next file if it is not the case
    if (nextEvent == nEvents)
    //^ This condition is also satisfied when the method is executed for the first time
    {
        bool const fileOpened = NextInputFile();
        
        if (not fileOpened)
            return false;
    }
    
    
    // Read ID of the next event from the tree
    eventIDTree->GetEntry(nextEvent);
    ++nextEvent;
    
    // Translate the ID from the storage format to the standard format of the framework
    eventID.Set(bfEventID.RunNumber(), bfEventID.LumiSectionNumber(), bfEventID.EventNumber());
    
    
    return true;
}

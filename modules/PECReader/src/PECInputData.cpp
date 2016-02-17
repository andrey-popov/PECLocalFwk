#include <PECFwk/PECReader/PECInputData.hpp>

#include <PECFwk/core/Logger.hpp>
#include <PECFwk/core/ROOTLock.hpp>

#include <algorithm>
#include <iterator>
#include <stdexcept>


using namespace logging;
using namespace std::string_literals;


PECInputData::PECInputData(std::string const name):
    ReaderPlugin(name),
    nextFileIt(inputFiles.end()),
    nEvents(0), nextEvent(0)
{}


PECInputData::~PECInputData()
{}


Plugin *PECInputData::Clone() const
{
    return new PECInputData(name);
}


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


EventID const &PECInputData::GetEventID() const
{
    return eventID;
}


std::unique_ptr<TTree> PECInputData::GetTree(std::string const &name)
{
    // Make sure that the requested tree is not the event ID tree which is read by this plugin
    if (name == "pecEventID/EventID")
        throw std::runtime_error("PECInputData::GetTree: Requested to read the tree with event "
         "ID, which is not allowed since the tree is read by the PECInputData plugin.");
    
    
    ROOTLock::Lock();
    std::unique_ptr<TTree> tree(dynamic_cast<TTree *>(curInputFile->Get(name.c_str())));
    ROOTLock::Unlock();
    
    return tree;
}


bool PECInputData::NextInputFile()
{
    // Delete the event ID tree and the current input file
    ROOTLock::Lock();
    eventIDTree.reset();
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
    
    eventIDTree.reset(dynamic_cast<TTree *>(curInputFile->Get("pecEventID/EventID")));
    
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

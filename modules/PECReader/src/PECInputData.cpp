#include <PECFwk/PECReader/PECInputData.hpp>

#include <PECFwk/core/ROOTLock.hpp>

#include <algorithm>
#include <iterator>
#include <stdexcept>


using namespace std::string_literals;


PECInputData::PECInputData(std::string const name /*= ""*/):
    ReaderPlugin((name == "") ? "InputData" : name),
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
    std::copy(srcFiles.begin(), srcFiles.end(), std::back_inserter(inputFiles));
    nextFileIt = inputFiles.begin();
}


void PECInputData::EndRun()
{}


TTree *PECInputData::GetTree(std::string const &name)
{
    ROOTLock::Lock();
    TTree *tree = dynamic_cast<TTree *>(curInputFile->Get(name.c_str()));
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
    
    
    return true;
}

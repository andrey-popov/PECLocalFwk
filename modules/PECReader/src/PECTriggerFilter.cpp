#include <PECFwk/PECReader/PECTriggerFilter.hpp>

#include <PECFwk/core/Processor.hpp>
#include <PECFwk/PECReader/PECInputData.hpp>

#include <algorithm>


using namespace std::literals::string_literals;


PECTriggerFilter::PECTriggerFilter(std::string const name /*= "PECTriggerFilter"*/):
    AnalysisPlugin(name),
    inputDataPluginName("InputData"), inputDataPlugin(nullptr),
    triggerTreeName("pecTrigger/TriggerInfo"), triggerTree(nullptr)
{}


PECTriggerFilter::~PECTriggerFilter()
{}


void PECTriggerFilter::BeginRun(Dataset const &)
{
    // Find the plugin that reads input files
    inputDataPlugin = dynamic_cast<PECInputData const *>(
      GetMaster().GetPluginBefore(inputDataPluginName, GetName()));
    
    // Register reading the tree with trigger information and by default disable all branches
    inputDataPlugin->LoadTree(triggerTreeName);
    triggerTree = inputDataPlugin->ExposeTree(triggerTreeName);
    triggerTree->SetBranchStatus("*", false);
}


double PECTriggerFilter::GetWeight() const
{
    return 1.;
}



// PECTriggerFilterData::PECTriggerFilterData(std::string const &name, C const &ranges);
// (Defined in the header)


// PECTriggerFilterData::PECTriggerFilterData(C const &ranges);
// (Defined in the header)


PECTriggerFilterData::~PECTriggerFilterData()
{}


void PECTriggerFilterData::BeginRun(Dataset const &dataset)
{
    // Make sure this is actually data
    if (dataset.IsMC())
        throw std::logic_error("PECTriggerFilterData::BeginRun: This class is intended to be used "
          "with data, but the input dataset is simulated.");
    
    
    // Set up the pointer to PECInputData and register reading of the trigger tree
    PECTriggerFilter::BeginRun(dataset);
}


Plugin *PECTriggerFilterData::Clone() const
{
    return new PECTriggerFilterData(*this);
}


// void PECTriggerFilterData::ConstructRanges(C const &ranges);
// (Defined in the header)


bool PECTriggerFilterData::ProcessEvent()
{
    // Check if the current trigger range includes the current event and update it if needed
    auto const &eventID = inputDataPlugin->GetEventID();
    
    if (not currentRange or not currentRange->InRange(eventID))
    {
        // Find the range that contains the current event
        auto res = std::find_if(ranges.begin(), ranges.end(),
         [&eventID](TriggerRange const *r){return (r->InRange(eventID));});
        
        if (res == ranges.end())
        // No range contains the given event ID
        {
            // Reset the current range and reject the event
            currentRange = nullptr;
            return false;
        }
        
        
        // A valid trigger range has been found. Get the corresponding branch of the tree and
        //assign the buffer to it
        currentRange = *res;
        triggerTree->SetBranchStatus("*", false);
        
        TBranch *branch =
         triggerTree->GetBranch((currentRange->GetDataTriggerPattern() + "__accept").c_str());
        
        if (not branch)
            throw std::runtime_error("PECTriggerFilterData::ProcessEvent: State of the trigger "s +
             "\"HLT_" + currentRange->GetDataTriggerPattern() + "_v*\" is not stored in the "
             "source tree.");
        
        branch->SetStatus(true);
        branch->SetAddress(&bfAccepted);
    }
    
    
    // Now as the tree has been set up propertly, read it
    inputDataPlugin->ReadEventFromTree(triggerTreeName);
    
    
    return bfAccepted;
}



// PECTriggerFilterMC(std::string const &name, C const &ranges);
// (Defined in the header)


// PECTriggerFilterMC(C const &ranges);
// (Defined in the header)


PECTriggerFilterMC::~PECTriggerFilterMC()
{
    delete [] buffer;
}


void PECTriggerFilterMC::BeginRun(Dataset const &dataset)
{
    // Make sure this is actually data
    if (not dataset.IsMC())
        throw std::logic_error("PECTriggerFilterMC::BeginRun: This class is intended to be used "
          "with simulation, but the input dataset is data.");
    
    
    // Set up the pointer to PECInputData and register reading of the trigger tree
    PECTriggerFilter::BeginRun(dataset);
    
    
    // Create buffers into which the trigger decision will be read. They are allocated in a single
    //block. The number of buffers cannot exceed ranges.size(), but it might be smaller if some
    //MC triggers are repeated in several TriggerRange objects. For the sake of simplicity and
    //performance, no attempt to count distinctive triggers is made, and an array of size
    //ranges.size() is allocated.
    buffer = new Bool_t[ranges.size()];
    
    
    // Set statuses and addresses of relevant branches of the trigger tree. Take into account that
    //the same trigger might be specified in several trigger ranges
    triggerTree->SetBranchStatus("*", false);
    unsigned curBufferIndex = 0;
    
    for (unsigned i = 0; i < ranges.size(); ++i)
    {
        std::string const &curTriggerName = ranges.at(i).first->GetMCTriggerPattern();
        
        
        // Try to find name of the current trigger in previous ranges
        unsigned iPrev = 0;
        
        for (; iPrev < i; ++iPrev)
            if (ranges.at(iPrev).first->GetMCTriggerPattern() == curTriggerName)
                break;
        
        
        if (iPrev == i)
        //^ This means that the current trigger has not been encountered before
        {
            // Get the corresponding branch of the tree
            TBranch *branch = triggerTree->GetBranch((curTriggerName + "__accept").c_str());
            
            if (not branch)
                throw std::runtime_error("PECTriggerFilterMC::BeginRun: State of the trigger "s +
                 "\"HLT_" + curTriggerName + "_v*\" is not stored in the source tree.");
            
            
            // Set the status and address of the branch
            branch->SetStatus(true);
            branch->SetAddress(buffer + curBufferIndex);
            
            // Store the address also in ranges
            ranges.at(i).second = buffer + curBufferIndex;
            
            
            // Increment the buffer counter
            ++curBufferIndex;
        }
        else
        //^ The trigger has already been encountered, the tree had already been adjusted
        {
            // Only need to set the pointer to buffer in ranges
            ranges.at(i).second = ranges.at(iPrev).second;
        }
    }
}


Plugin *PECTriggerFilterMC::Clone() const
{
    return new PECTriggerFilterMC(*this);
}


double PECTriggerFilterMC::GetWeight() const
{
    double weight = 0.;
    
    for (auto const &r: ranges)
        if (*r.second)
        //^ The trigger in this range has fired
        {
            weight += r.first->GetLuminosity();
            //^ It is also possible to account for the trigger scale factor here, multiplying the
            //effective luminosity by it
        }
    
    
    return weight;
}


bool PECTriggerFilterMC::ProcessEvent()
{
    // Check all the requested triggers
    for (auto const &r: ranges)
        if (*r.second)
        //^ The event is accepted by the MC trigger in this range
            return true;
    
    // The workflow reaches this point if only no trigger accepts the event
    return false;
}

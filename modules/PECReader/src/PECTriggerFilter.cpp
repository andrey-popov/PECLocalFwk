#include <mensura/PECReader/PECTriggerFilter.hpp>

#include <mensura/core/Processor.hpp>
#include <mensura/core/ROOTLock.hpp>
#include <mensura/PECReader/PECInputData.hpp>

#include <algorithm>


using namespace std::literals::string_literals;


PECTriggerFilter::PECTriggerFilter(std::string const name /*= "PECTriggerFilter"*/):
    AnalysisPlugin(name),
    inputDataPluginName("InputData"), inputDataPlugin(nullptr),
    triggerTreeName("pecTrigger/TriggerInfo"), triggerTree(nullptr)
{}


void PECTriggerFilter::BeginRun(Dataset const &)
{
    // Find the plugin that reads input files
    inputDataPlugin = dynamic_cast<PECInputData const *>(GetDependencyPlugin(inputDataPluginName));
    
    // Register reading the tree with trigger information and by default disable all branches
    inputDataPlugin->LoadTree(triggerTreeName);
    triggerTree = inputDataPlugin->ExposeTree(triggerTreeName);
}


double PECTriggerFilter::GetWeight() const
{
    return 1.;
}



// PECTriggerFilterData::PECTriggerFilterData(std::string const &name, C const &ranges);
// (Defined in the header)


// PECTriggerFilterData::PECTriggerFilterData(C const &ranges);
// (Defined in the header)


void PECTriggerFilterData::BeginRun(Dataset const &dataset)
{
    // Make sure this is actually data
    if (dataset.IsMC())
        throw std::logic_error("PECTriggerFilterData::BeginRun: This class is intended to be used "
          "with data, but the input dataset is simulated.");
    
    
    // Set up the pointer to PECInputData and register reading of the trigger tree
    PECTriggerFilter::BeginRun(dataset);
    
    // Reset the current range in case this is not the first dataset processed by this instance of
    //the plugin. Without this, the plugin would not set up reading of the appropriate branches
    currentRange = nullptr;
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
        
        ROOTLock::Lock();
        triggerTree->SetBranchStatus("*", false);
        
        TBranch *branch =
         triggerTree->GetBranch((currentRange->GetDataTriggerPattern() + "__accept").c_str());
        
        if (not branch)
        {
            ROOTLock::Unlock();
            throw std::runtime_error("PECTriggerFilterData::ProcessEvent: State of the trigger "s +
             "\"HLT_" + currentRange->GetDataTriggerPattern() + "_v*\" is not stored in the "
             "source tree.");
        }
        
        branch->SetStatus(true);
        branch->SetAddress(&bfAccepted);
        ROOTLock::Unlock();
    }
    
    
    // Now as the tree has been set up propertly, read it
    inputDataPlugin->ReadEventFromTree(triggerTreeName);
    
    
    return bfAccepted;
}



PECTriggerFilterMC::TriggerWithBuffer::TriggerWithBuffer(TriggerRange const *trigger_) noexcept:
    trigger(trigger_)
{}


// PECTriggerFilterMC(std::string const &name, C const &ranges);
// (Defined in the header)


// PECTriggerFilterMC(C const &ranges);
// (Defined in the header)


void PECTriggerFilterMC::BeginRun(Dataset const &dataset)
{
    // Make sure this is actually data
    if (not dataset.IsMC())
        throw std::logic_error("PECTriggerFilterMC::BeginRun: This class is intended to be used "
          "with simulation, but the input dataset is data.");
    
    
    // Set up the pointer to PECInputData and register reading of the trigger tree
    PECTriggerFilter::BeginRun(dataset);
    
    
    // Create buffers into which trigger decisions will be read. After the map is filled, it is not
    //modified, and thus it is possible to operate with pointers to its values
    buffers.clear();
    
    for (auto const &r: ranges)
    {
        for (auto const &triggerPattern: r.trigger->GetMCTriggers())
            buffers[triggerPattern];
            //^ Note that same trigger pattern can be included in more than one range
    }
    
    
    // Set statuses and addresses of relevant branches of the trigger tree
    ROOTLock::Lock();
    triggerTree->SetBranchStatus("*", false);
    
    for (auto &b: buffers)
    {
        // Get the corresponding branch of the tree and make sure it exists
        TBranch *branch = triggerTree->GetBranch((b.first + "__accept").c_str());
        
        if (not branch)
        {
            ROOTLock::Unlock();
            throw std::runtime_error("PECTriggerFilterMC::BeginRun: State of the trigger "s +
             "\"HLT_" + b.first + "_v*\" is not stored in the source tree.");
        }
        
        
        // Set the status and address of the branch
        branch->SetStatus(true);
        branch->SetAddress(&b.second);
    }
    
    ROOTLock::Unlock();
    
    
    // In each TriggerRange wrapper, save pointers to associated buffers. This is needed to be
    //able to evaluate whether at least one trigger in a given range has accepted an event
    for (auto &r: ranges)
    {
        r.buffers.clear();
        
        for (auto const &triggerPattern: r.trigger->GetMCTriggers())
            r.buffers.emplace_back(&buffers[triggerPattern]);
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
    {
        // Check if at least one trigger in the current range has accepted the event
        bool accept = false;
        
        for (auto const &b: r.buffers)
            if (*b)
            {
                accept = true;
                break;
            }
        
        
        // Increase the weight with the contribution from the current range if the event is
        //accepted
        if (accept)
        {
            weight += r.trigger->GetLuminosity();
            //^ It is also possible to account for the trigger scale factor here, multiplying the
            //effective luminosity by it
        }
    }
    
    
    return weight;
}


bool PECTriggerFilterMC::ProcessEvent()
{
    // Read branches with trigger decisions
    inputDataPlugin->ReadEventFromTree(triggerTreeName);
    
    
    // Check if at least of the buffers equals true, which would mean that the event is accepted by
    //at least one trigger
    for (auto const &b: buffers)
    {
        if (b.second == true)
            return true;
    }
    
    
    // If control reaches this point, the event has not been accepted by any trigger
    return false;
}

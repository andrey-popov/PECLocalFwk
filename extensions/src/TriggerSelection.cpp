#include <TriggerSelection.hpp>

#include <TObjString.h>

#include <stdexcept>


using namespace std;


TriggerRangeWrapper::TriggerRangeWrapper(TriggerRange const &data_):
    data(data_), index(0), passTrigger(false)
{}


TriggerSelectionData::TriggerSelectionData(list<TriggerRangeWrapper> &ranges_):
    ranges(ranges_),
    currentRange(ranges.begin())
{}


bool TriggerSelectionData::PassTrigger(EventID const &eventID, TClonesArray const *names,
 Bool_t const *fired) const
{
    auto const &endRange = ranges.end();
    
    // Update the current trigger range if it is not compatible with the given event
    if (not currentRange->data.InRange(eventID))
        // Loop over all the available ranges
        for (currentRange = ranges.begin(); ; ++currentRange)
        {
            if (currentRange == endRange)
                // No one range contains the given event. The event is rejected
                return false;
            
            if (currentRange->data.InRange(eventID))
                break;
        }
    
    
    // Update the trigger index if it refers to a trigger with wrong name
    string const &pattern = currentRange->data.GetDataTriggerPattern();
    string name("");
    unsigned const nNames = names->GetEntries();
    
    if (currentRange->index < nNames)
        name = (dynamic_cast<TObjString *>(names->At(currentRange->index)))->String().Data();
    
    if (name.find(pattern) == string::npos)
        for (currentRange->index = 0; ; ++(currentRange->index))
        {
            if (currentRange->index == nNames)
                // The requested trigger was not found in the menu
                throw runtime_error(string("TriggerSelectionData::PassTrigger: The requested "
                 "trigger pattern \"") + currentRange->data.GetDataTriggerPattern() +
                 "\" was not found.");
            
            name = (dynamic_cast<TObjString *>(names->At(currentRange->index)))->String().Data();
            
            if (name.find(pattern) not_eq string::npos)
                break;
        }
    
    
    // Now currentRange::index refers to the correct trigger
    return fired[currentRange->index];
}


double TriggerSelectionData::GetWeight(PECReader const &reader) const
{
    return currentRange->data.PassEventSelection(reader);
}



TriggerSelectionMC::TriggerSelectionMC(std::list<TriggerRangeWrapper> &ranges_):
    ranges(ranges_)
{}


void TriggerSelectionMC::UpdateTriggerIndices(TClonesArray const *names)
{
    unsigned const nNames = names->GetEntries();
    
    // Loop over all the trigger ranges
    for (auto &range: ranges)
    {
        string const &pattern = range.data.GetMCTriggerPattern();
        
        // Find the index of the trigger name which matches the pattern
        for (range.index = 0; ; ++range.index)
        {
            if (range.index == nNames)
                throw runtime_error(string("TriggerSelectionMC::PassTrigger: The requested "
                 "trigger pattern \"") + range.data.GetMCTriggerPattern() + "\" was not "
                 "found.");
            
            string name((dynamic_cast<TObjString *>(names->At(range.index)))->String().Data());
            
            if (name.find(pattern) not_eq string::npos)
                break;
        }
    }
}


bool TriggerSelectionMC::PassTrigger(Bool_t const *fired) const
{
    // Save the information about the fired triggers and logical OR of all these booleans
    bool result = false;
    
    for (auto &range: ranges)
    {
        range.passTrigger = fired[range.index];
        
        if (range.passTrigger)
            result = true;
    }
    
    
    // The event is rejected if only it fires no MC trigger in the given list of TriggerRange
    //objects
    return result;
}


double TriggerSelectionMC::GetWeight(PECReader const &reader) const
{
    double weight = 0.;
    
    for (auto const &range: ranges)
    {
        if (range.passTrigger and range.data.PassEventSelection(reader))
            weight += range.data.GetLuminosity() * ScaleFactor(range.data, reader);
    }
    
    
    return weight;
}


double TriggerSelectionMC::ScaleFactor(TriggerRange const &, PECReader const &) const
{
    return 1.;
}



TriggerSelection::TriggerSelection(std::list<TriggerRange> const &ranges):
    TriggerSelection(ranges.begin(), ranges.end())
{}


TriggerSelection::TriggerSelection(TriggerSelection &&src):
    ranges(move(src.ranges)),
    dataSelection(move(src.dataSelection)),
    mcSelection(move(src.mcSelection)),
    isData(src.isData), isReset(src.isReset)
{}


TriggerSelection::TriggerSelection(TriggerSelection const &src):
    TriggerSelectionInterface(),
    ranges(src.ranges),
    dataSelection(new TriggerSelectionData(ranges)),
    mcSelection(new TriggerSelectionMC(ranges)),
    isData(src.isData), isReset(src.isReset)
{}


void TriggerSelection::NewFile(bool isData_) const
{
    isData = isData_;
    isReset = true;
}


bool TriggerSelection::PassTrigger(EventID const &eventID, TClonesArray const *names,
 Bool_t const *fired) const
{
    if (isData)
        return dataSelection->PassTrigger(eventID, names, fired);
    else
    {
        if (isReset)
        {
            mcSelection->UpdateTriggerIndices(names);
            isReset = false;
        }
        
        return mcSelection->PassTrigger(fired);
    }
}


double TriggerSelection::GetWeight(PECReader const &reader) const
{
    if (isData)
        return dataSelection->GetWeight(reader);
    else
        return mcSelection->GetWeight(reader);
}


TriggerSelectionInterface *TriggerSelection::Clone() const
{
    return new TriggerSelection(*this);
}
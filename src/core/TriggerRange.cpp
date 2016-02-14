#include <PECFwk/core/TriggerRange.hpp>

#include <boost/algorithm/string/predicate.hpp>


using namespace std;


TriggerRange::TriggerRange(unsigned long firstRun, unsigned long lastRun,
 string const &dataTriggerPattern_, double intLumi_, string const &MCTriggerPattern_):
    firstEvent(firstRun, true), lastEvent(lastRun, false),
    dataTriggerPattern(GetTriggerBasename(dataTriggerPattern_)), intLumi(intLumi_),
    MCTriggerPattern(GetTriggerBasename(MCTriggerPattern_))
{}


void TriggerRange::SetRange(EventID const &firstEvent_, EventID const &lastEvent_)
{
    firstEvent = firstEvent_;
    lastEvent = lastEvent_;
}


void TriggerRange::SetDataTrigger(string const &pattern, double intLumi_)
{
    dataTriggerPattern = GetTriggerBasename(pattern);
    intLumi = intLumi_;
}


void TriggerRange::SetMCTrigger(string const &pattern)
{
    MCTriggerPattern = GetTriggerBasename(pattern);
}


void TriggerRange::SetEventSelection(function<bool(PECReader const &)> eventSelection_)
{
    eventSelection = eventSelection_;
}


bool TriggerRange::InRange(EventID const &eventID) const
{
    return (firstEvent <= eventID and eventID <= lastEvent);
}


bool TriggerRange::PassEventSelection(PECReader const &reader) const
{
    if (eventSelection)  // i.e. there is a valid event selection defined
        return eventSelection(reader);
    else
        return true;
}


string const &TriggerRange::GetDataTriggerPattern() const
{
    return dataTriggerPattern;
}


string const &TriggerRange::GetMCTriggerPattern() const
{
    return MCTriggerPattern;
}


double TriggerRange::GetLuminosity() const
{
    return intLumi;
}


string TriggerRange::GetTriggerBasename(string const &name)
{
    // The name might (or might not) contain a prefix "HLT_" and/or a postfix with version
    //number of the form "_v\d+", "_v\*", or "_v". They are stripped off if found
    string basename(name);
    
    // First, the prefix
    if (boost::starts_with(basename, "HLT_"))
        basename = basename.substr(4);
    
    // Now check the postfix
    if (boost::ends_with(basename, "_v*"))
        basename = basename.substr(0, basename.length() - 3);
    else if (boost::ends_with(basename, "_v"))
        basename = basename.substr(0, basename.length() - 2);
    else
    {
        // Maybe, the full version was specified
        int pos = basename.length() - 1;
        
        while (pos >= 0 and basename[pos] >= '0' and basename[pos] <= '9')
            --pos;
        
        if (pos >= 2 and basename[pos] == 'v' and basename[pos - 1] == '_')
            basename = basename.substr(0, pos - 1);
    }
    
    
    return basename;
}

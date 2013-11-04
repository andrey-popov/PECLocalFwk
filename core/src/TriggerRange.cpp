#include <TriggerRange.hpp>


using namespace std;


TriggerRange::TriggerRange(unsigned long firstRun, unsigned long lastRun,
 string const &dataTriggerPattern_, double intLumi_, string const &MCTriggerPattern_):
    firstEvent(firstRun, true), lastEvent(lastRun, false),
    dataTriggerPattern(dataTriggerPattern_), intLumi(intLumi_), MCTriggerPattern(MCTriggerPattern_)
{}


void TriggerRange::SetRange(EventID const &firstEvent_, EventID const &lastEvent_)
{
    firstEvent = firstEvent_;
    lastEvent = lastEvent_;
}


void TriggerRange::SetDataTrigger(string const &pattern, double intLumi_)
{
    dataTriggerPattern = pattern;
    intLumi = intLumi_;
}


void TriggerRange::SetMCTrigger(string const &pattern)
{
    MCTriggerPattern = pattern;
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

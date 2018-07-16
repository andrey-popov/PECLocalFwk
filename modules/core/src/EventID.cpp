#include <mensura/core/EventID.hpp>


EventID::EventID():
    runNumber(0), lumiBlockNumber(0), eventNumber(0), bxNumber(0)
{}


EventID::EventID(RunNumber_t runNumber_, LumiBlockNumber_t lumiBlockNumber_,
  EventNumber_t eventNumber_, BXNumber_t bxNumber_ /*= 0*/):
    runNumber(runNumber_), lumiBlockNumber(lumiBlockNumber_), eventNumber(eventNumber_),
    bxNumber(bxNumber_)
{}


EventID::EventID(RunNumber_t runNumber_, bool minimal /*= true*/)
{
    Set(runNumber_, minimal);
}


void EventID::Set(RunNumber_t runNumber_, LumiBlockNumber_t lumiBlockNumber_,
  EventNumber_t eventNumber_, BXNumber_t bxNumber_ /*= 0*/)
{
    runNumber = runNumber_;
    lumiBlockNumber = lumiBlockNumber_;
    eventNumber = eventNumber_;
    bxNumber = bxNumber_;
}


void EventID::Set(RunNumber_t runNumber_, bool minimal /*= true*/)
{
    runNumber = runNumber_;
    
    if (minimal)
    {
        lumiBlockNumber = 0;
        eventNumber = 0;
    }
    else
    {
        lumiBlockNumber = -1;
        eventNumber = -1;
    }
}


bool EventID::operator<(EventID const &rhs) const
{
    if (runNumber != rhs.runNumber)
        return (runNumber < rhs.runNumber);
    
    if (lumiBlockNumber != rhs.lumiBlockNumber)
        return (lumiBlockNumber < rhs.lumiBlockNumber);
    
    return (eventNumber < rhs.eventNumber);
}


bool EventID::operator==(EventID const &rhs) const
{
    return (runNumber == rhs.runNumber and lumiBlockNumber == rhs.lumiBlockNumber and
     eventNumber == rhs.eventNumber);
}


bool EventID::operator<=(EventID const &rhs) const
{
    return not(rhs < *this);
}


EventID::RunNumber_t EventID::Run() const
{
    return runNumber;
}


EventID::LumiBlockNumber_t EventID::LumiBlock() const
{
    return lumiBlockNumber;
}


EventID::EventNumber_t EventID::Event() const
{
    return eventNumber;
}


EventID::BXNumber_t EventID::BunchCrossing() const
{
    return bxNumber;
}

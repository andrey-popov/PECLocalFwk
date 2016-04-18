#include <mensura/extensions/TriggerRange.hpp>

#include <boost/algorithm/string/predicate.hpp>

#include <sstream>
#include <stdexcept>


TriggerRange::TriggerRange(unsigned long firstRun, unsigned long lastRun,
  std::initializer_list<std::string> const &dataTriggers_, double intLumi_,
  std::initializer_list<std::string> const &mcTriggers_):
    firstEvent(firstRun, true), lastEvent(lastRun, false),
    intLumi(intLumi_)
{
    FillTriggerSet(dataTriggers, dataTriggers_);
    FillTriggerSet(mcTriggers, mcTriggers_);
}


TriggerRange::TriggerRange(unsigned long firstRun, unsigned long lastRun,
  std::string const &dataTrigger, double intLumi_, std::string const &mcTrigger):
    firstEvent(firstRun, true), lastEvent(lastRun, false), intLumi(intLumi_)
{
    dataTriggers.emplace(GetTriggerBasename(dataTrigger));
    mcTriggers.emplace(GetTriggerBasename(mcTrigger));
}


std::set<std::string> const &TriggerRange::GetDataTriggers() const
{
    return dataTriggers;
}


std::string const &TriggerRange::GetDataTriggerPattern() const
{
    return *dataTriggers.begin();
}


double TriggerRange::GetLuminosity() const
{
    return intLumi;
}


std::set<std::string> const &TriggerRange::GetMCTriggers() const
{
    return mcTriggers;
}


std::string const &TriggerRange::GetMCTriggerPattern() const
{
    return *mcTriggers.begin();
}


bool TriggerRange::InRange(EventID const &eventID) const
{
    return (firstEvent <= eventID and eventID <= lastEvent);
}


void TriggerRange::SetDataTriggers(std::initializer_list<std::string> const &dataTriggers_,
  double intLumi_)
{
    FillTriggerSet(dataTriggers, dataTriggers_);
    intLumi = intLumi_;
}


void TriggerRange::SetMCTriggers(std::initializer_list<std::string> const &mcTriggers_)
{
    FillTriggerSet(mcTriggers, mcTriggers_);
}


void TriggerRange::SetRange(EventID const &firstEvent_, EventID const &lastEvent_)
{
    firstEvent = firstEvent_;
    lastEvent = lastEvent_;
}


void TriggerRange::FillTriggerSet(std::set<std::string> &target,
  std::initializer_list<std::string> const &triggerNames)
{
    for (auto const &pattern: triggerNames)
    {
        auto const res = target.emplace(GetTriggerBasename(pattern));
        
        if (not res.second)
        {
            std::ostringstream message;
            message << "TriggerRange::FillTriggerSet: Found a duplicate for provided trigger " <<
              "trigger pattern \"" << pattern << "\", which translates into representation \"" <<
              GetTriggerBasename(pattern) << "\".";
            throw std::runtime_error(message.str());
        }
    }
}


std::string TriggerRange::GetTriggerBasename(std::string const &name)
{
    // The name might (or might not) contain a prefix "HLT_" and/or a postfix with version
    //number of the form "_v\d+", "_v\*", or "_v". They are stripped off if found
    std::string basename(name);
    
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

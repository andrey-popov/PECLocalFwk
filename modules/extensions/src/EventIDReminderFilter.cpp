#include <mensura/extensions/EventIDReminderFilter.hpp>

#include <mensura/core/EventIDReader.hpp>
#include <mensura/core/Processor.hpp>


EventIDReminderFilter::EventIDReminderFilter(std::string const &name,
  unsigned maxReminder_, unsigned denominator_, bool isReversed_ /*=false*/):
    AnalysisPlugin(name),
    eventIDPluginName("EventID"), eventIDPlugin(nullptr),
    maxReminder(maxReminder_), denominator(denominator_), isReversed(isReversed_)
{}


EventIDReminderFilter::EventIDReminderFilter(unsigned maxReminder_, unsigned denominator_,
  bool isReversed_ /*=false*/):
    AnalysisPlugin("EventIDReminderFilter"),
    eventIDPluginName("EventID"), eventIDPlugin(nullptr),
    maxReminder(maxReminder_), denominator(denominator_), isReversed(isReversed_)
{}


EventIDReminderFilter::~EventIDReminderFilter()
{}


void EventIDReminderFilter::BeginRun(Dataset const &)
{
    // Save pointer to plugin that produces jets
    eventIDPlugin = dynamic_cast<EventIDReader const *>(GetDependencyPlugin(eventIDPluginName));
}


Plugin *EventIDReminderFilter::Clone() const
{
    return new EventIDReminderFilter(*this);
}


bool EventIDReminderFilter::ProcessEvent()
{
    auto const &id = eventIDPlugin->GetEventID();
    bool const res = ((id.Event() % denominator) <= maxReminder);
    
    return (isReversed) ? not res : res;
}

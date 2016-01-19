#include <FilterEventIDReminderPlugin.hpp>

#include <Processor.hpp>

#include <stdexcept>
#include <fstream>
#include <sstream>


using namespace std;


FilterEventIDReminderPlugin::FilterEventIDReminderPlugin(string const &name_,
 unsigned maxReminder_, unsigned denominator_, bool isReversed_ /*=false*/):
    AnalysisPlugin(name_),
    maxReminder(maxReminder_), denominator(denominator_), isReversed(isReversed_)
{}


Plugin *FilterEventIDReminderPlugin::Clone() const
{
    return new FilterEventIDReminderPlugin(name, maxReminder, denominator, isReversed);
}


void FilterEventIDReminderPlugin::BeginRun(Dataset const &)
{
    // Save pointer to the reader plugin
    reader = dynamic_cast<PECReaderPlugin const *>(processor->GetPluginBefore("Reader", name));
}


void FilterEventIDReminderPlugin::EndRun()
{}


bool FilterEventIDReminderPlugin::ProcessEvent()
{
    auto const &id = (*reader)->GetEventID();
    bool const res = ((id.Event() % denominator) <= maxReminder);
    
    return (isReversed) ? not res : res;
}

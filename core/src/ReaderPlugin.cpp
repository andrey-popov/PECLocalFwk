#include <ReaderPlugin.hpp>


ReaderPlugin::ReaderPlugin(std::string const &name):
    Plugin(name)
{}


ReaderPlugin::~ReaderPlugin()
{}


Plugin::EventOutcome ReaderPlugin::ReinterpretDecision(bool decision) const
{
    if (decision)
        return EventOutcome::Ok;
    else
        return EventOutcome::NoEvents;
}

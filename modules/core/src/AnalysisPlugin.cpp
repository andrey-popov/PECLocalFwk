#include <mensura/core/AnalysisPlugin.hpp>


AnalysisPlugin::AnalysisPlugin(std::string const &name):
    Plugin(name)
{}


AnalysisPlugin::~AnalysisPlugin()
{}


Plugin::EventOutcome AnalysisPlugin::ReinterpretDecision(bool decision) const
{
    if (decision)
        return EventOutcome::Ok;
    else
        return EventOutcome::FilterFailed;
}

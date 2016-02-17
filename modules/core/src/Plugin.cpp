#include <PECFwk/core/Plugin.hpp>

#include <stdexcept>


using namespace std;


Plugin::Plugin(string const &name_):
    name(name_), master(nullptr)
{}


Plugin::~Plugin()
{}


void Plugin::SetMaster(Processor const *processor)
{
    master = processor;
}


Processor const &Plugin::GetMaster() const
{
    if (not master)
    {
        throw logic_error("Plugin::GetMaster: The plugin does not have a master.");
    }
    
    return *master;
}


string const &Plugin::GetName() const
{
    return name;
}


void Plugin::BeginRun(Dataset const &)
{}


void Plugin::EndRun()
{}


Plugin::EventOutcome Plugin::ProcessEventToOutcome()
{
    return ReinterpretDecision(ProcessEvent());
}

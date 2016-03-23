#include <PECFwk/core/Plugin.hpp>

#include <PECFwk/core/Processor.hpp>

#include <sstream>
#include <stdexcept>


using namespace std;


Plugin::Plugin(string const &name_):
    name(name_), master(nullptr)
{}


Plugin::~Plugin()
{}


void Plugin::BeginRun(Dataset const &)
{}


void Plugin::EndRun()
{}


Processor const &Plugin::GetMaster() const
{
    if (not master)
    {
        throw logic_error("Plugin::GetMaster: Plugin \""s + name + "\" does not have a master.");
    }
    
    return *master;
}


string const &Plugin::GetName() const
{
    return name;
}


void Plugin::SetMaster(Processor const *processor)
{
    master = processor;
}


Plugin const *Plugin::GetDependencyPlugin(std::string const &nameOfDependency) const
{
    Plugin const *p = GetMaster().GetPluginBeforeQuiet(nameOfDependency, name);
    
    if (not p)
    {
        std::ostringstream ost;
        ost << "Plugin::GetDependencyPlugin: Failed to find plugin \"" << nameOfDependency <<
          "\" among dependencies of plugin \"" << name << "\".";
        
        throw std::runtime_error(ost.str());
    }
    
    return p;
}


Plugin::EventOutcome Plugin::ProcessEventToOutcome()
{
    return ReinterpretDecision(ProcessEvent());
}

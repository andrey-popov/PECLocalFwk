#include <mensura/core/Processor.hpp>

#include <mensura/core/Logger.hpp>
#include <mensura/core/ROOTLock.hpp>
#include <mensura/core/RunManager.hpp>

#include <iostream>
#include <mutex>
#include <sstream>
#include <stdexcept>
#include <thread>


using namespace std;
using namespace logging;


Processor::PluginInPath::PluginInPath(Plugin *plugin_):
    plugin(plugin_),
    lastResult(true)
{}


Plugin *Processor::PluginInPath::operator->() const
{
    return plugin.get();
}


Processor::Processor(Processor &&src) noexcept:
    manager(src.manager),
    services(move(src.services)),
    path(move(src.path)),
    pluginNameMap(move(src.pluginNameMap))
{
    // Prevent the source object from deleting the plugins
    src.path.clear();
    
    
    // Update the master in services and plugins
    for (auto &s: services)
        s.second->SetMaster(this);
    
    for (auto &p: path)
        p->SetMaster(this);
}


Processor::Processor(Processor const &src):
    manager(src.manager),
    pluginNameMap(src.pluginNameMap)
{
    // Copy services and plugins
    for (auto const &s: src.services)
        services.emplace(piecewise_construct,
         forward_as_tuple(s.first), forward_as_tuple(s.second->Clone()));
    
    for (auto const &p: src.path)
    {
        path.emplace_back(p.plugin->Clone());
        path.back().dependencies = p.dependencies;
    }
    
    
    // Update the master in services and plugins
    for (auto &s: services)
        s.second->SetMaster(this);
    
    for (auto &p: path)
        p->SetMaster(this);
}


Processor::~Processor() noexcept
{
    // Destroy plugins in a reversed order
    for (auto pIt = path.rbegin(); pIt != path.rend(); ++pIt)
        pIt->plugin.reset();
    
    // Do not care in which order services will be destroyed, so let it be done automatically
}


void Processor::RegisterService(Service *service)
{
    // Attempt to add the given service to the collection
    auto res = services.insert(make_pair(service->GetName(), unique_ptr<Service>(service)));
    
    
    // Throw an exception if the insersion failed
    if (res.second == false)
        throw runtime_error("Processor::RegisterService: Attempting to register a second "s +
         "service with name \"" + service->GetName() + "\".");
    
    
    // Introduce this to the service
    service->SetMaster(this);
}


void Processor::RegisterPlugin(Plugin *plugin,
  std::initializer_list<std::string> const &dependencies)
{
    // Make sure there is no plugin with the same name
    if (pluginNameMap.find(plugin->GetName()) != pluginNameMap.end())
        throw std::runtime_error("Processor::RegisterPlugin: Attempting to register a second "s +
         "plugin named \"" + plugin->GetName() + "\".");
    
    
    // Make sure all dependencies have been registered and save their indices
    std::vector<unsigned> depIndices;
    
    for (auto const &depName: dependencies)
    {
        auto const pIt = pluginNameMap.find(depName);
        
        if (pIt == pluginNameMap.end())
        {
            std::ostringstream message;
            message << "Processor::RegisterPlugin: Dependency \"" << depName <<
              "\" of plugin \"" << plugin->GetName() << "\" is not registered.";
            throw std::runtime_error(message.str());
        }
        else
            depIndices.emplace_back(pIt->second);
    }
    
    
    // Update the map for plugin names
    pluginNameMap[plugin->GetName()] = path.size();  // This will be the index of the new plugin
    
    
    // Insert the plugin into the path
    path.emplace_back(plugin);
    path.back().dependencies = depIndices;
    
    
    // Set this as its master
    plugin->SetMaster(this);
}


void Processor::RegisterPlugin(Plugin *plugin)
{
    // Make sure there is no plugin with the same name
    if (pluginNameMap.find(plugin->GetName()) != pluginNameMap.end())
        throw std::runtime_error("Processor::RegisterPlugin: Attempting to register a second "s +
         "plugin named \"" + plugin->GetName() + "\".");
    
    
    // Update the map for plugin names
    unsigned const curIndex = path.size();  // This will be the index of the new plugin
    pluginNameMap[plugin->GetName()] = curIndex;
    
    // Insert the plugin into the path
    path.emplace_back(plugin);
    
    // Declare that the new plugin depends on the previous one, unless this is the first plugin in
    //the path
    if (curIndex > 0)
        path.back().dependencies.push_back(curIndex - 1);
    
    
    // Introduce this to the plugin
    plugin->SetMaster(this);
}


void Processor::operator()()
{
    // Make sure there is a manager
    if (not manager)
        throw std::logic_error("Processor::operator(): This method cannot be executed when no "
          "RunManager has been specified.");
    
    
    // Read datasets from the queue in the manager one by one
    while (true)
    {
        // Safely pop a dataset from the queue
        manager->mutexDatasets.lock();
        
        if (manager->datasets.empty())  // no more datasets to process
        {
            manager->mutexDatasets.unlock();
            return;
        }
        
        Dataset dataset(manager->datasets.front());
        manager->datasets.pop();
        manager->mutexDatasets.unlock();
        
        
        // Process the dataset
        ProcessDataset(dataset);
    }
}


void Processor::OpenDataset(Dataset const &dataset)
{
    // Declare begin of a dataset for all services and plugins
    for (auto &s: services)
        s.second->BeginRun(dataset);
     
    for (auto &p: path)
        p->BeginRun(dataset);
}


Plugin::EventOutcome Processor::ProcessEvent()
{
    // Variable to store outcome of the current event
    Plugin::EventOutcome result = Plugin::EventOutcome::NoEvents;
    //^ This initialization allows to terminate processing of the dataset when there are no plugins
    //registered
    
    
    // Process event with all plugins
    for (auto &p: path)
    {
        // Determine whether the current plugin should be executed for the current event
        bool runPlugin = true;
        
        for (auto const &depIndex: p.dependencies)
        {
            if (not path[depIndex].lastResult)
            {
                runPlugin = false;
                break;
            }
        }
        
        
        // Execute the plugin and save its result. If it is not executed, the result is set to
        //false to prevent execution of other plugins that depend on it
        if (runPlugin)
        {
            result = p->ProcessEventToOutcome();
            p.lastResult = (result == Plugin::EventOutcome::Ok);
        }
        else
            p.lastResult = false;
        
        
        // Stop executing the path if a reader has returned false
        if (result == Plugin::EventOutcome::NoEvents)
            break;
    }
    
    
    // If some reader said that there are no events left in the current dataset, declare end of
    //run for all plugins and services
    if (result == Plugin::EventOutcome::NoEvents)
    {
        // Plugins are processed in a reversed order of their creation
        for (auto pIt = path.rbegin(); pIt != path.rend(); ++pIt)
            (*pIt)->EndRun();
        
        for (auto &s: services)
            s.second->EndRun();
    }
    
    
    // Return the outcome of this event
    return result;
}


void Processor::ProcessDataset(Dataset const &dataset)
{
    logger << timestamp << "Start processing source file \"" <<
     dataset.GetFiles().front().GetBaseName() << ".root\"." << eom;
    
    OpenDataset(dataset);
    
    while (ProcessEvent() != Plugin::EventOutcome::NoEvents)
    {}
}


Service const *Processor::GetService(string const &name) const
{
    auto res = services.find(name);
    
    if (res == services.end())
        throw runtime_error("Processor::GetService: A service with name \""s + name +
         "\" is not registered.");
    else
        return res->second.get();
}


Service const *Processor::GetServiceQuiet(string const &name) const
{
    auto res = services.find(name);
    
    if (res == services.end())
        return nullptr;
    else
        return res->second.get();
}


Plugin const *Processor::GetPlugin(string const &name) const
{
    return path.at(GetPluginIndex(name)).plugin.get();
}


Plugin const *Processor::GetPluginQuiet(string const &name) const noexcept
{
    try
    {
        return GetPlugin(name);
    }
    catch (...)
    {
        return nullptr;
    }
}


Plugin const *Processor::GetPluginBefore(string const &name, string const &dependentName) const
{
    unsigned const indexInterest = GetPluginIndex(name);
    unsigned const indexDependent = GetPluginIndex(dependentName);
    
    if (indexDependent <= indexInterest)  // dependency is broken
        throw logic_error("Processor::GetPluginBefore: Requested plugin is executed after the "
         "dependent plugin.");
    
    return path.at(indexInterest).plugin.get();
}


Plugin const *Processor::GetPluginBeforeQuiet(string const &name, string const &dependentName)
  const noexcept
{
    try
    {
        return GetPluginBefore(name, dependentName);
    }
    catch (...)
    {
        return nullptr;
    }
}


void Processor::SetManager(RunManager *manager_)
{
    manager = manager_;
}


unsigned Processor::GetPluginIndex(string const &name) const
{
    unsigned index;
    
    try
    {
        index = pluginNameMap.at(name);
    }
    catch (out_of_range)  // the name has not been found
    {
        // Re-throw the exception with a more detailed error message
        throw out_of_range(string("Processor::GetPluginIndex: ") + "No plugin with name \"" + name +
         "\" is present in the path.");
    }
    
    return index;
}

#include <mensura/core/Processor.hpp>

#include <mensura/core/Logger.hpp>
#include <mensura/core/ROOTLock.hpp>
#include <mensura/core/RunManager.hpp>

#include <iostream>
#include <mutex>
#include <stdexcept>
#include <thread>


using namespace std;
using namespace logging;


Processor::Processor(RunManager *manager_ /*= nullptr*/) noexcept:
    manager(manager_)
{}


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
        path.emplace_back(p->Clone());
    
    
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
        pIt->reset();
    
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


void Processor::RegisterPlugin(Plugin *plugin)
{
    // Make sure there is no plugin named the same
    if (pluginNameMap.find(plugin->GetName()) != pluginNameMap.end())
        throw runtime_error(string("Processor::RegisterPlugin: Attempting to register a second ") +
         "plugin named \"" + plugin->GetName() + "\".");
    
    
    // Update the map for plugin names
    pluginNameMap[plugin->GetName()] = path.size();  // this will be the index of this plugin
    
    // Insert the plugin into the path
    path.emplace_back(plugin);
    
    
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
    for (auto &plugin: path)
    {
        result = plugin->ProcessEventToOutcome();
        
        if (result != Plugin::EventOutcome::Ok)
        {
            // Whatever happened, do not execute remaining plugins for the current event
            break;
        }
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
    return path.at(GetPluginIndex(name)).get();
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
    
    return path.at(indexInterest).get();
}


Plugin const *Processor::GetPluginBeforeQuiet(string const &name, string const &dependentName) const
 noexcept
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

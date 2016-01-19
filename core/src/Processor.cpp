#include <Processor.hpp>

#include <RunManager.hpp>
#include <Plugin.hpp>
#include <ROOTLock.hpp>
#include <Logger.hpp>

#include <thread>
#include <mutex>
#include <stdexcept>
#include <iostream>


using namespace std;
using namespace logging;


Processor::Processor() noexcept:
    manager(nullptr)
{}


Processor::Processor(RunManager *manager_):
    manager(manager_)
{
    // Create the reader plugin
    RegisterPlugin(new PECReaderPlugin(move(manager->readerConfig)));
}


Processor::Processor(Processor &&src) noexcept:
    manager(src.manager),
    path(move(src.path)),
    nameMap(move(src.nameMap))
{
    // Prevent the source object from deleting the plugins
    src.path.clear();
}


Processor::Processor(Processor const &src):
    manager(src.manager),
    nameMap(src.nameMap)
{
    for (auto const &p: src.path)
        path.emplace_back(p->Clone());
}


Processor::~Processor() noexcept
{
    // Destroy plugins in a reversed order
    for (auto pIt = path.rbegin(); pIt != path.rend(); ++pIt)
        pIt->reset();
}


void Processor::RegisterPlugin(Plugin *plugin)
{
    // Make sure there is no plugin named the same
    if (nameMap.find(plugin->GetName()) != nameMap.end())
        throw runtime_error(string("Processor::RegisterPlugin: Attempting to register a second ") +
         "plugin named \"" + plugin->GetName() + "\".");
    
    
    // Update the map for plugin names
    nameMap[plugin->GetName()] = path.size();  // this will be the index of this plugin
    
    
    // Insert the plugin into the path
    path.emplace_back(plugin);
}


void Processor::operator()()
{
    // Register this as the master of the owned plugins
    for (auto &p: path)
        p->SetMaster(this);
    
    
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


void Processor::ProcessDataset(Dataset const &dataset)
{
    /*cout << "Thread " << this_thread::get_id() << " starts processing dataset with file \"" <<
     dataset.GetFiles().front().name << "\"\n";*/
    logger << timestamp << "Start processing source file \"" <<
     dataset.GetFiles().front().GetBaseName() << ".root\"." << eom;
    
    
    // Declare begin of a dataset for all plugins
    for (auto pIt = path.begin(); pIt != path.end(); ++pIt)
        (*pIt)->BeginRun(dataset);
    
    
    // Process all events in the dataset
    while (true)
    {
        Plugin::EventOutcome result = Plugin::EventOutcome::NoEvents;
        //^ This initialization allows to break the event loop if there are no plugins registered
        
        for (auto &plugin: path)
        {
            result = plugin->ProcessEventToOutcome();
            
            if (result != Plugin::EventOutcome::Ok)
            {
                // Whatever happened, do not execute remaining plugins for the current event
                break;
            }
        }
        
        
        // Check if any event has been read successfully during this iteration
        if (result == Plugin::EventOutcome::NoEvents)
        {
            // Some reader said that there were no events left in the current dataset
            break;
        }
    }
    
    
    // Declare end of the dataset for all plugins (in a reversed order)
    for (auto pIt = path.rbegin(); pIt != path.rend(); ++pIt)
        (*pIt)->EndRun();
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
        index = nameMap.at(name);
    }
    catch (out_of_range)  // the name has not been found
    {
        // Re-throw the exception with a more detailed error message
        throw out_of_range(string("Processor::GetPluginIndex: ") + "No plugin with name \"" + name +
         "\" is present in the path.");
    }
    
    return index;
}
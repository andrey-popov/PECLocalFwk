/**
 * \file Processor.hpp
 * 
 * The module defines a class responsible for evaluation of a set of requested plugins.
 */

#pragma once

#include <PECFwk/core/Dataset.hpp>
#include <PECFwk/core/PluginForward.hpp>
#include <PECFwk/core/RunManagerForward.hpp>
#include <PECFwk/core/Service.hpp>

#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


/**
 * \class Processor
 * \brief Runs plugins in a single thread
 * 
 * An instance of this class performs evaluation of requested plugins. A plugin can access other
 * plugins in the path via constant pointers. The plugins are owned by an instance of class
 * Processor; move and copy constructors are implemented to respect this ownership.
 * 
 * One instance of class Processor is expected to be run in a single (separate) thread. The class
 * is friend to class RunManager and pops up datasets from a queue RunManager::datasets.
 */
class Processor
{
public:
    /// Constructor
    Processor(RunManager *manager = nullptr) noexcept;
    
    /// Move constructor
    Processor(Processor &&src) noexcept;
    
    /**
     * \brief Copy constructor
     * 
     * Configuration of plugins is copied only but not their states. Copying of an instance of
     * class Processor after it started processing a dataset is not foreseen and is not a well-
     * defined operation.
     */
    Processor(Processor const &src);
    
    /// Assignment operator is deleted
    Processor &operator=(Processor const &) = delete;
    
    /// Destructor
    ~Processor() noexcept;

public:
    /**
     * \brief Adds a new service
     * 
     * The service is owned by Processor. All services must have unique names, and an exception
     * will be thrown if a duplicate is found.
     */
    void RegisterService(Service *service);
    
    /**
     * \brief Adds a new plugin to be executed
     * 
     * The new plugin is inserted at the end of the execution path. The plugin object is owned by
     * Processor.
     */
    void RegisterPlugin(Plugin *plugin);
    
    /// Entry point for execution when processing is run by RunManager
    void operator()();
    
    /**
     * \brief Processes a single atomic dataset
     * 
     * Processes the dataset with registered plugins. For each event the plugins are executed
     * in the same order as they have been registered. If ProcessEvent method of a plugin
     * returns false, the following plugins are not evaluated for the event.
     */
    void ProcessDataset(Dataset const &dataset);
    
    /**
     * \brief Returns pointer to service with given name
     * 
     * The method uses a map to find the service and this is fast. Throws an except if a service
     * with the given name does not exist.
     */
    Service const *GetService(std::string const &name) const;
    
    /**
     * \brief Returns pointer to service with given name
     * 
     * The behaviour is identical to method GetService, but it does not throw an except if the
     * requested service is not found. Instead, a null pointer is returned.
     */
    Service const *GetServiceQuiet(std::string const &name) const;
    
    /**
     * \brief Returns pointer to plugin with given name
     * 
     * Searches for a plugin with given name in the path and returns a constant pointer to it
     * if found; an exception is thrown otherwise. Do not use this method when an ordering of
     * plugins in the path is important (e.g. when one plugin needs to access information from
     * another one, which then must be placed in the path before the former plugin); consider
     * GetPluginBefore instead.
     */
    Plugin const *GetPlugin(std::string const &name) const;
    
    /**
     * \brief Returns a pointer to plugin with given name
     * 
     * The behaviour is identical to GetPlugin, but it does not throw an exeption if the plugin
     * is not found. Instead a null pointer is returned.
     */
    Plugin const *GetPluginQuiet(std::string const &name) const noexcept;
    
    /**
     * \brief Returns a pointer to plugin with given name. In addition checks that the plugin
     * is placed in the path before a plugin with name dependentName.
     * 
     * Searches the path for a plugin with given name and returns a constant pointer to it.
     * Verifies that the plugin is placed in the path before a dependent plugin with name
     * dependentName. If either of the plugins is not found or the dependency condition is
     * broken, thrown an exception. The method is intended to be called from one plugin to
     * access information from another one the former plugin depends on.
     */
    Plugin const *GetPluginBefore(std::string const &name, std::string const &dependentName)
     const;
    
    /**
     * \brief Returns a pointer to plugin with given name. In addition checks that the plugin
     * is placed in the path before a plugin with name dependentName.
     * 
     * Behaviour is identical to GetPluginBefore, but it does not throw an exception if the
     * requested plugin is not found. Instead the method returns a null pointer.
     */
    Plugin const *GetPluginBeforeQuiet(std::string const &name,
     std::string const &dependentName) const noexcept;

private:
    /// Retuns index in the path of a plugin with given name. Throws an exception if not found
    unsigned GetPluginIndex(std::string const &name) const;

private:
    /**
     * \brief Parent RunManager instance
     * 
     * The pointed-to object is not owned by this.
     */
    RunManager *manager;
    
    /**
     * \brief Registered services
     * 
     * The map is utilized both for storage and to provide a fast access to services by their names.
     * The ordering of services is not important to logic of the framework.
     */
    std::map<std::string, std::unique_ptr<Service>> services;
    
    /**
     * \brief Pointers to registered plugins
     * 
     * Random access is mandatory for this container.
     */
    std::vector<std::unique_ptr<Plugin>> path;
    
    /// Mapping from plugin names to their indices in vector path
    std::unordered_map<std::string, unsigned> pluginNameMap;
};

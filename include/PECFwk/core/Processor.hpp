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
 * This class is responsible for execution of a collection of plugins, which is organized into an
 * ordered path. It allows each plugin to access other plugins in the path and services. Plugins
 * and services are owned by Processor, which also registers itself as their master.
 * 
 * Instances of this class are spanned by RunManager to process a queue of datasets. Each processor
 * is run in a separate thread. This class is a friend of RunManager and thus can pop up datasets
 * from the queue RunManager::datasets.
 * 
 * However, it is also possible to run a Processor independently of a RunManager. In this case the
 * entry point for execution is method ProcessDataset.
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
     * Configurations of each plugin and service are copied, but not their states. Pointers to
     * master are changed to this. Copying a Processor after it started processing a dataset is not
     * supported and would lead to an undefined behaviour.
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
     * \brief Processes a dataset
     * 
     * Processes the dataset with registered plugins. For each event the plugins are executed
     * in the same order as they have been registered. If ProcessEvent method of an analysis plugin
     * returns false, subsequent plugins in the path are not evaluated for the event.
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
     * The map is utilized both for storage and to provide a fast access to services by their
     * names. The ordering of services is not important to logic of the framework.
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

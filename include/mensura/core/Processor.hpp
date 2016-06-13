#pragma once

#include <mensura/core/Dataset.hpp>
#include <mensura/core/Plugin.hpp>
#include <mensura/core/Service.hpp>

#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>


class RunManager;


/**
 * \class Processor
 * \brief Runs plugins in a single thread
 * 
 * This class is responsible for execution of a collection of plugins, which is organized into an
 * ordered path. It allows each plugin to access other plugins in the path and services. Plugins
 * and services are owned by Processor, which also registers itself as their master.
 * 
 * Each plugin can depend on an arbitrary number (including zero) of plugins preceding it in the
 * path. For each event it is executed if only method ProcessEvent() of none of its dependencies
 * has returned false. If a reader plugin declares that there are no events left, processing of the
 * current dataset is stopped immediately.
 * 
 * Instances of this class are spanned by RunManager to process a queue of datasets. Each processor
 * is run in a separate thread. The entry point for execution is operator(). This class is a friend
 * of RunManager and profits from this to pop up datasets from the queue RunManager::datasets.
 * 
 * It is also possible to run a Processor independently of a RunManager. In this case method
 * OpenDataset must be called before the event loop, and then each single event can be processed
 * with method ProcessEvent. The dataset as a whole can be processed using method ProcessDataset.
 */
class Processor
{
private:
    /// An auxiliary structure to decorate a plugin with information about its behaviour in a path
    struct PluginInPath
    {
        /**
         * \brief Constructor
         * 
         * This takes ownership of the given plugin.
         */
        PluginInPath(Plugin *plugin);
        
        /// Simplifies calling methods for the underlying plugin
        Plugin *operator->() const;
        
        /// Plugin
        std::unique_ptr<Plugin> plugin;
        
        /**
         * \brief Indices of other plugins in the path on which this one depends
         * 
         * Indices refer to vector path. Dependencies must precede with plugin.
         */
        std::vector<unsigned> dependencies;
        
        /// Indicates whether last time this plugin was executed it returned status Ok
        bool lastResult;
    };
    
public:
    /// Default constructor
    Processor() = default;
    
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
     * \brief Adds a new plugin with explicit dependencies to the execution path
     * 
     * The plugin is added at the end of the path, and its dependencies are set up according to the
     * given collection of plugin names. If one of the dependencies is not found or if a plugin
     * with the same name has already been registered, an exception is thrown. Processor takes the
     * ownership of the plugin object.
     */
    void RegisterPlugin(Plugin *plugin, std::initializer_list<std::string> const &dependencies);
    
    /**
     * \brief Adds a new plugin to the execution path
     * 
     * The new plugin is added at the end of the path. The previous plugin in the path (if any) is
     * set as its only dependency. If a plugin with the same name has already been registered, an
     * exception is thrown. Processor takes the ownership of the plugin object.
     */
    void RegisterPlugin(Plugin *plugin);
    
    /// Entry point for execution when processing is run by RunManager
    void operator()();
    
    /**
     * \brief Initialization for a new dataset
     * 
     * Calls BeginRun for all plugins and services.
     */
    void OpenDataset(Dataset const &dataset);
    
    /**
     * \brief Processes the next event in the dataset
     * 
     * Plugins are executed in the order of their appearance in the path. A plugin can prevent
     * execution of subsequent plugins in the path, implementing event filtering (in case of
     * analysis plugins) or declaring that there are no events left in the dataset (in case of
     * readers). If a reader declares end of dataset, this method calls EndRun for all plugins and
     * services. The return value is the outcome code of the last executed plugin, and thus
     * indicates whether the event has been rejected by a filter or the dataset has run out of
     * events.
     */
    Plugin::EventOutcome ProcessEvent();
    
    /**
     * \brief Processes a dataset
     * 
     * This short-cut calls OpenDataset and exectutes ProcessEvent in a loop.
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
    
    /// Sets owning RunManager
    void SetManager(RunManager *manager);
    
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
     * \brief Execution path, which contains registered plugins
     * 
     * Random access is mandatory for this container.
     */
    std::vector<PluginInPath> path;
    
    /// Mapping from plugin names to their indices in vector path
    std::unordered_map<std::string, unsigned> pluginNameMap;
};

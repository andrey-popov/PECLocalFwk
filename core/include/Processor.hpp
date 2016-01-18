/**
 * \file Processor.hpp
 * 
 * The module defines a class responsible for evaluation of a set of requested plugins.
 */

#pragma once

#include <PluginForward.hpp>
#include <PECReaderConfig.hpp>
#include <PECReaderPlugin.hpp>
#include <RunManagerForward.hpp>
#include <Dataset.hpp>

#include <vector>
#include <string>
#include <unordered_map>
#include <memory>


/**
 * \class Processor
 * \brief Runs PECReader and a set of requested plugins in a single thread
 * 
 * An instance of this class performs evaluation of requested plugins. A plugin can access other
 * plugins in the path via constant pointers. The plugins are owned by an instance of class
 * Processor; move and copy constructors are implemented to respect this ownership.
 * 
 * Plugin "Reader" of type PECReaderPlugin is automatically constructed and inserted at the
 * beginning of the path. Its configuration is moved from the parent instance of class RunManager.
 * 
 * One instance of class Processor is expected to be run in a single (separate) thread. The class
 * is friend to class RunManager and pops up datasets from a queue RunManager::datasets.
 */
class Processor
{
public:
    /// Default constructor
    Processor() noexcept;
    
    /**
     * \brief Constructor
     * 
     * Constructor moves RunManager::readerConfig to this.
     */
    Processor(RunManager *manager);
    
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
     * \brief Adds a new plugin to be executed
     * 
     * The new plugin is inserted at the end of execution path. The plugin object is owned by
     * Processor; therefore, the user might need to use std::move to transfer the ownship.
     * Note that a plugin wrapper for class PECReader is included automatically and executed
     * first; it must not be registered explicitly.
     */
    void RegisterPlugin(Plugin *plugin);
    
    /// Entry point for execution
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
     * \brief Returns a pointer to plugin with given name
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
     * \brief Pointers to registered plugins
     * 
     * Random access is mandatory for this container. The first plugin is always a wrapper for
     * PECReader class.
     */
    std::vector<std::unique_ptr<Plugin>> path;
    
    /// Mapping from plugin names to their indices in vector path
    std::unordered_map<std::string, unsigned> nameMap;
};
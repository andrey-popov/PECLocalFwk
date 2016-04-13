#pragma once

#include <mensura/core/Dataset.hpp>
#include <mensura/core/Plugin.hpp>
#include <mensura/core/Service.hpp>

#include <queue>
#include <mutex>
#include <memory>


class Processor;


/**
 * \class RunManager
 * \brief Performs parallel processing of datasets
 * 
 * The class hosts a list of atomic (containing a single file each) datasets and a set of user-
 * defined plugins and manages a thread pool that processes the datasets. It only forwards
 * parameters, and the actual processing is delegated to instances of dedicated class Processor.
 * 
 * Some of data members are accessed directly by the friend class Processor.
 */
class RunManager
{
public:
    /// Constructor from a container of instances of Dataset
    template<typename InputIt>
    RunManager(InputIt const &datasetsBegin, InputIt const &datasetsEnd);
    
    /// Copy constructor is deleted
    RunManager(RunManager const &) = delete;
    
    /// Assignment operator is deleted
    RunManager &operator=(RunManager const &) = delete;

public:
    /// Processes datasets with a pool of nThreads threads
    void Process(int nThreads);
    
    /**
     * \brief Processes datasets with a pool of threads
     * 
     * Number of threads is determined by multiplying total supported number of concurrent
     * threads by the given fraction.
     */
    void Process(double loadFraction);
    
    /**
     * \brief Adds a new service
     * 
     * The service is owned by RunManager. Names of all registered services must be unique;
     * otherwise an exception will be thrown in consequitive initialization.
     */
    void RegisterService(Service *service);
    
    /**
     * \brief Adds a new plugin to be executed
     * 
     * The new plugin is inserted at the end of execution path. The plugin object is owned by
     * RunManager. Note that a plugin wrapper for class PECReader is included automatically and
     * executed first; it must not be registered explicitly.
     */
    void RegisterPlugin(Plugin *plugin);

private:
    /// Implementation for famility public methods Process
    void ProcessImp(int nThreads);

private:
    /// Atomic (containing a single file each) datasets
    std::queue<Dataset> datasets;
    
    /// A mutex to lock container with atomic datasets
    std::mutex mutexDatasets;
    
    /**
     * \brief Vector of registered services
     * 
     * The order of services is not important to the framework.
     */
    std::vector<std::unique_ptr<Service>> services;
    
    /// Vector of registered plugins
    std::vector<std::unique_ptr<Plugin>> plugins;

friend class Processor;
};


template<typename InputIt>
RunManager::RunManager(InputIt const &datasetsBegin, InputIt const &datasetsEnd)
{
    // Fill container with atomic datasets
    for (InputIt d = datasetsBegin; d != datasetsEnd; ++d)
    {
        for (auto const &file: d->GetFiles())
        {
            datasets.push(d->CopyParameters());
            datasets.back().AddFile(file);
        }
    }
}

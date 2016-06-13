#pragma once

#include <mensura/core/Dataset.hpp>
#include <mensura/core/Plugin.hpp>
#include <mensura/core/Processor.hpp>
#include <mensura/core/Service.hpp>

#include <initializer_list>
#include <queue>
#include <mutex>
#include <memory>


/**
 * \class RunManager
 * \brief Performs parallel processing of datasets
 * 
 * The class hosts a list of atomic (containing a single file each) datasets and manages a thread
 * pool that processes them. It only forwards parameters, and the actual processing is delegated to
 * instances of dedicated class Processor.
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
     * Directly calls Processor::RegisterService for the underlying template processor. Consult
     * documentation for that method.
     */
    void RegisterService(Service *service);
    
    /**
     * \brief Adds a new plugin with explicit dependencies to the execution path
     * 
     * Directly calls Processor::RegisterPlugin for the underlying template processor. Consult
     * documentation for that method.
     */
    void RegisterPlugin(Plugin *plugin, std::initializer_list<std::string> const &dependencies);
    
    /**
     * \brief Adds a new plugin to the execution path
     * 
     * Directly calls Processor::RegisterPlugin for the underlying template processor. Consult
     * documentation for that method.
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
    
    /// A template processor to which services and plugins are registered
    Processor templateProcessor;
    
friend class Processor;
};


template<typename InputIt>
RunManager::RunManager(InputIt const &datasetsBegin, InputIt const &datasetsEnd)
{
    templateProcessor.SetManager(this);
    
    
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

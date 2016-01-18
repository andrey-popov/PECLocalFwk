/**
 * \file RunManager.hpp
 * 
 * Module describes a class to process datasets in parallel
 */

#pragma once

#include <Dataset.hpp>
#include <Plugin.hpp>
#include <ProcessorForward.hpp>
#include <PECReaderConfig.hpp>

#include <queue>
#include <mutex>
#include <memory>


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
     * \brief Returns a reference to local configuration object for PECReader
     * 
     * Method enables the user to adjust configuration that is forwarded to instances of class
     * PECReader.
     */
    PECReaderConfig &GetPECReaderConfig();
    
    /**
     * \brief Adds a new plugin to be executed
     * 
     * The new plugin is inserted at the end of execution path. The plugin object is owned by
     * RunManager; therefore, the user might need to use std::move to transfer the ownship.
     * Note that a plugin wrapper for class PECReader is included automatically and executed
     * first; it must not be registered explicitly.
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
    
    /// Configuration for PECReader
    std::unique_ptr<PECReaderConfig> readerConfig;
    
    /// Vector of registered plugins
    std::vector<std::unique_ptr<Plugin>> plugins;

friend class Processor;
};


template<typename InputIt>
RunManager::RunManager(InputIt const &datasetsBegin, InputIt const &datasetsEnd):
    readerConfig(new PECReaderConfig)
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
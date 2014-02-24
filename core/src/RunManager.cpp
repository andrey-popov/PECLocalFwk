#include <RunManager.hpp>

#include <Processor.hpp>
#include <Logger.hpp>

#include <thread>
#include <vector>
#include <functional>
#include <stdexcept>

#include <iostream>


using namespace std;
using namespace logging;


//RunManager::RunManager(InputIt const &begin, const &InputIt end);
//^ Described in the header file


void RunManager::Process(int nThreads)
{
    // Simply forward to the implementation
    ProcessImp(nThreads);
}


void RunManager::Process(double loadFraction)
{
    // Number of supported concurrent threads (note that it is only a hint)
    unsigned const nMaxThreads = thread::hardware_concurrency();
    
    // Call the implementation
    ProcessImp(loadFraction * nMaxThreads);
}


PECReaderConfig &RunManager::GetPECReaderConfig()
{
    return *readerConfig.get();
}


void RunManager::RegisterPlugin(Plugin *plugin)
{
    plugins.emplace_back(unique_ptr<Plugin>(plugin));
}


void RunManager::ProcessImp(int nThreads)
{
    // Check number of threads for adequacy
    if (nThreads < 1)
        throw runtime_error("RunManager::ProcessImp: Requested number of threads is less than "
         "one.");
    
    if (nThreads > int(datasets.size()))
        nThreads = datasets.size();
    
    
    // Create processing objects. The first one is constructed from this, others are copy-
    //constructed from the first one
    vector<Processor> processors;
    processors.reserve(nThreads);
    processors.emplace_back(this);
    
    for (auto &p: plugins)
        processors.front().RegisterPlugin(p.release());
    
    for (int i = 1; i < nThreads; ++i)
        processors.emplace_back(processors.front());
    
    
    // Put the processors into separate threads
    vector<thread> threads;
    
    for (auto &p: processors)
        threads.emplace_back(move(p));
    
    
    // Wait for them to finish
    for (auto &t: threads)
        t.join();
    
    logger << timestamp << "All files have been processed." << eom;
}
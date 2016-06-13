#include <mensura/core/RunManager.hpp>

#include <mensura/core/Logger.hpp>

#include <iomanip>
#include <iostream>
#include <functional>
#include <stdexcept>
#include <thread>
#include <vector>


using namespace std;
using namespace logging;


RunManager::PluginStat::PluginStat(std::string const &pluginName_):
    pluginName(pluginName_),
    numVisited(0), numPassed(0)
{}


//RunManager::RunManager(InputIt const &begin, const &InputIt end);
//^ Described in the header file


void RunManager::PrintSummary() const
{
    std::cout << std::setw(20) << std::left << "Plugin" << "    " <<
      std::setw(9) << std::right << "Visited" << "    " << std::setw(9) << "Passed\n";
    
    for (auto const &stat: pathStat)
        std::cout << std::setw(20) << std::left << stat.pluginName << "    " <<
          std::setw(9) << std::right << stat.numVisited << "   " << std::setw(9) << stat.numPassed <<
          std::endl;
}


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


void RunManager::RegisterService(Service *service)
{
    templateProcessor.RegisterService(service);
}


void RunManager::RegisterPlugin(Plugin *plugin,
  std::initializer_list<std::string> const &dependencies)
{
    templateProcessor.RegisterPlugin(plugin, dependencies);
}


void RunManager::RegisterPlugin(Plugin *plugin)
{
    templateProcessor.RegisterPlugin(plugin);
}


void RunManager::ProcessImp(int nThreads)
{
    // Check number of threads for adequacy
    if (nThreads < 1)
        throw runtime_error("RunManager::ProcessImp: Requested number of threads is less than "
         "one.");
    
    if (nThreads > int(datasets.size()))
        nThreads = datasets.size();
    
    
    // Create processing objects. The template processor is used as the first one, others are copy-
    //constructed from it
    vector<Processor> processors;
    processors.reserve(nThreads);
    processors.emplace_back(std::move(templateProcessor));
    
    
    // Other processors are copied from the first one
    for (int i = 1; i < nThreads; ++i)
        processors.emplace_back(processors.front());
    
    
    // Put the processors into separate threads
    vector<thread> threads;
    
    for (auto &p: processors)
        threads.emplace_back(std::ref(p));
    
    
    // Wait for them to finish
    for (auto &t: threads)
        t.join();
    
    logger << timestamp << "All files have been processed." << eom;
    
    
    // Save plugin statistics
    std::vector<std::string> const pluginNames = processors.front().GetPath();
    pathStat.reserve(pluginNames.size());
    
    for (auto const &pluginName: pluginNames)
    {
        PluginStat stat(pluginName);
        
        for (auto const &p: processors)
        {
            auto const counts = p.GetStat(pluginName);
            stat.numVisited += counts.first;
            stat.numPassed += counts.second;
        }
        
        pathStat.emplace_back(std::move(stat));
    }
}

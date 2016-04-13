#include <mensura/core/ROOTLock.hpp>


std::mutex ROOTLock::globalROOTMutex;


void ROOTLock::Lock()
{
    globalROOTMutex.lock();
}


void ROOTLock::Unlock()
{
    globalROOTMutex.unlock();
}


bool ROOTLock::TryLock()
{
    return globalROOTMutex.try_lock();
}
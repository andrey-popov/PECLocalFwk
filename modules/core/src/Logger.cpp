#include <PECFwk/core/Logger.hpp>

#include <stdexcept>
#include <ctime>
#include <string>


using namespace std;
using namespace logging;


_EndOfMessage logging::eom()
{
    return _EndOfMessage();
}


_TimeStamp logging::timestamp()
{
    return _TimeStamp();
}


// A globally-available instance
Logger logging::logger;


// Definition for static data members
recursive_mutex Logger::outputMutex;
bool Logger::isLocked = false;


Logger &Logger::operator<<(_EndOfMessage (*)())
{
    // Lock the mutex and write the new-line symbol
    VerifyLock();
    cout << endl;
    
    // Unlock the mutex
    isLocked = false;
    outputMutex.unlock();
    
    return *this;
}


Logger &Logger::operator<<(_TimeStamp (*)())
{
    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);
    
    char stamp[100];
    strftime(stamp, 100, "%a %b %e %T %G", timeinfo);
    
    *this << '[' << stamp << "] ";
    
    return *this;
}


void Logger::VerifyLock()
{
    // First lock the mutex
    outputMutex.lock();
    
    // Check if it has already been locked by the current thread
    if (isLocked)
        outputMutex.unlock();  // make the mutex locked only one time
    else
        isLocked = true;
}
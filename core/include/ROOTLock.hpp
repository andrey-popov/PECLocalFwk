/**
 * \file ROOTLock.hpp
 * \author Andrey Popov
 * 
 * The module defines a global locking mechanism to protect thread-unsafe ROOT routines.
 */

#pragma once

#include <mutex>


/**
 * \class ROOTLock
 * \brief Provides a lock to protect thread-unsafe ROOT routines
 * 
 * The class is a simple wrapper around a static mutex. Since ROOT is not thread-safe, this lock
 * must be used to mark all the crical blocks of code that call ROOT routines. In particular,
 * creation of any ROOT objects must be guarded with the help of this class. The user should also
 * keep in mind that threads share the current ROOT directory object (gDirectory).
 */
class ROOTLock
{
    public:
        /// Constructor is deleted
        ROOTLock() = delete;
    
    public:
        /// Locks
        static void Lock();
        
        /// Unlocks
        static void Unlock();
        
        /// Checks if the lock is set
        static bool TryLock();
    
    private:
        /// Static mutex
        static std::mutex globalROOTMutex;
};
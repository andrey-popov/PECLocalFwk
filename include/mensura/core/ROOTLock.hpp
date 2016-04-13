#pragma once

#include <mutex>


/**
 * \class ROOTLock
 * \brief Provides a lock to protect ROOT routines that are not thread-safe
 * 
 * This class is a simple wrapper around a static mutex. In general, ROOT is not thread-safe, and
 * a number of operations must be protected by a lock. Their list includes (but is likely not
 * limited to) the following:
 *   * creation of objects inheriting from TObject, especially, TFile, TTree, and histograms;
 *   * deletion of these objects;
 *   * calling TTree::SetBranchAddress or TBranch::SetAddress.
 * User should also remember that threads share the current ROOT directory object (gDirectory).
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

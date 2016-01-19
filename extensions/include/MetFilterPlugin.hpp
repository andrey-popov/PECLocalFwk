/**
 * \file MetFilterPlugin.hpp
 *
 * Defines a simple plugin to perform additional filtering on value of MET.
 */

#pragma once

#include <AnalysisPlugin.hpp>

#include <PECReaderPlugin.hpp>


/**
 * \class MetFilterPlugin
 * \brief Performs additional selection on value of MET
 * 
 * The plugin checks that the value of MET in an event is larger than the given threshold.
 */
class MetFilterPlugin: public AnalysisPlugin
{
public:
    /**
     * \brief Constructor
     */
    MetFilterPlugin(double threshold);
    
public:
    /**
     * \brief Constructs a newly-initialised copy
     * 
     * Consult documentation of the overriden method for details.
     */
    Plugin *Clone() const;
    
    /**
     * \brief Notifies this that a dataset has been opened
     * 
     * Only updates the pointer to the reader plugin.
     */
    void BeginRun(Dataset const &);
    
private:
    /// Peforms filtering
    bool ProcessEvent();
    
private:
    /// Pointer to the reader plugin
    PECReaderPlugin const *reader;
    
    /// Threshold on MET
    double threshold;
};

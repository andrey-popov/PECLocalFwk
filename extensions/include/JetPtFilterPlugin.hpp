/**
 * \file JetPtFilterPlugin.hpp
 *
 * Defines a simple plugin to perform additional filtering on jet transverse momenta.
 */

#pragma once

#include <Plugin.hpp>

#include <PECReaderPlugin.hpp>


/**
 * \class JetPtFilterPlugin
 * \brief Performs additional selection on jet transverse momenta
 * 
 * The plugin checks that an event contains at least a given number of jets above a given threshold
 * on transverse momentum.
 */
class JetPtFilterPlugin: public Plugin
{
public:
    /**
     * \brief Constructor
     */
    JetPtFilterPlugin(unsigned minNumJets, double ptThreshold);
    
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
    
    /// Peforms filtering
    bool ProcessEvent();
    
private:
    /// Pointer to the reader plugin
    PECReaderPlugin const *reader;
    
    /// Minimal number of jets passing the threshold
    unsigned minNumJets;
    
    /// Threshold on jet transverse momentum
    double ptThreshold;
};

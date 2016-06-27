#pragma once

#include <mensura/core/ReaderPlugin.hpp>

#include <string>


/**
 * \class GeneratorReader
 * \brief Abstract base class for a reader plugin that provides process-level generator information
 * 
 * Provides an interface to access nominal and alternative generator-level event weights.
 */
class GeneratorReader: public ReaderPlugin
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    GeneratorReader(std::string const name = "Generator");
    
public:
    /**
     * \brief Returns alternative LHE-level weight with the given index
     * 
     * Unconditionally throws an exception in the default implementation.
     */
    virtual double GetAltWeight(unsigned index) const;
    
    /// Returns nominal LHE-level weight for the current event
    virtual double GetNominalWeight() const = 0;
    
    /**
     * \brief Returns number of available alternative weights
     * 
     * In the default implementation this number is always zero.
     */
    virtual unsigned GetNumAltWeights() const;
};

#pragma once

#include <mensura/core/ReaderPlugin.hpp>

#include <string>
#include <utility>


/**
 * \class GeneratorReader
 * \brief Abstract base class for a reader plugin that provides process-level generator information
 * 
 * Provides an interface to access event energy scale, information to evaluate PDF, nominal and
 * alternative generator-level event weights.
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
    
    /**
     * \brief Returns PDG ID of particles sampled from PDF
     * 
     * Unconditionally throws an exception in the default implementation.
     */
    virtual std::pair<int, int> GetPdfPart() const;
    
    /**
     * \brief Returns Bjorken x for particles sampled from PDF
     * 
     * Unconditionally throws an exception in the default implementation.
     */
    virtual std::pair<double, double> GetPdfX() const;
    
    /**
     * \brief Returns nominal energy scale of the event, in GeV
     * 
     * Unconditionally throws an exception in the default implementation.
     */
    virtual double GetScale() const;
};

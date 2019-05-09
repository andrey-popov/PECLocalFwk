#pragma once

#include <mensura/AnalysisPlugin.hpp>

#include <vector>


/**
 * \class EventWeightPlugin
 * \brief An abstract base class for a plugin that calculates event weights
 * 
 * This class defines an interface to access event weight and its systematic variations. A derived
 * class must provide an implementation for method ProcessEvent that fills the collection of
 * weights in a way that complies with the specification provided in the documentation for method
 * GetWeights.
 */
class EventWeightPlugin: public AnalysisPlugin
{
public:
    /// Constructs an instance with the given name
    EventWeightPlugin(std::string const &name);
    
public:
    /// Returns the nominal weight
    double GetWeight() const;
    
    /// Returns the number of systematic variations
    unsigned GetNumVariations() const noexcept;
    
    /**
     * \brief Returns an "up" systematic variation with the given index
     * 
     * The indices start from zero. If the given index is larger than the total number of
     * variations, an exception is thrown.
     */
    double GetWeightUp(unsigned iSource) const;
    
    /**
     * \brief Returns a "down" systematic variation with the given index
     * 
     * The indices start from zero. If the given index is larger than the total number of
     * variations, an exception is thrown.
     */
    double GetWeightDown(unsigned iSource) const;
    
    /**
     * \brief Returns a vector with nominal weight and all systematic variations
     * 
     * The vector is always non-empty and contains an odd number of elements. The first element is
     * the nominal weight, it is followed by an "up" variation for the first source of systematic
     * uncertainty, a "down" variation for this source, and so on.
     */
    std::vector<double> const &GetWeights() const noexcept;
    
protected:
    /**
     * \brief Weights assigned to the current event
     * 
     * The first weight is the nominal one, and it is followed by (optional) systematic variations.
     * Consult documentation for method GetWeights for details.
     */
    std::vector<double> weights;
};

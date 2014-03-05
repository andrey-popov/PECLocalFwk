/**
 * \file EventWeightPlugin.hpp
 * \author Andrey Popov
 * 
 * Defines an abstract base class for a plugin to calculate event weight.
 */

#include <Plugin.hpp>

#include <vector>


/**
 * \class EventWeightPlugin
 * \brief An abstract base class for a plugin to calculate event weight
 * 
 * An interface to calculate event weight and its systematical variations. A derived class must
 * provide an implementation for ProcessEvent method that will fill the collection of weights in a
 * reasonable way. The content of the collection must follow a specification provided in the
 * documentation for method GetWeights.
 */
class EventWeightPlugin: public virtual Plugin
{
public:
    /**
     * \brief Constructor
     * 
     * Forwards the given name to the constructor of Plugin.
     */
    EventWeightPlugin(std::string const &name);
    
    /// Copy constructor
    EventWeightPlugin(EventWeightPlugin const &src) noexcept;
    
    /// Trivial destructor
    virtual ~EventWeightPlugin() noexcept;
    
public:
    /// Returns the nominal weight
    double GetWeight() const;
    
    /// Returns the number of sources of systematical uncertainty
    unsigned GetNumSystSources() const noexcept;
    
    /**
     * \brief Returns an "up" systematical variation of the weight caused by the source with the
     * given index.
     * 
     * The indices start from zero. If the given index is larger than the total number of sources of
     * systematical uncertainty, an exception is thrown. The name of the variation is conventional
     * and does not imply that the returned with is larger than the nominal one.
     */
    double GetWeightUp(unsigned iSource) const;
    
    /**
     * \brief Returns a "down" systematical variation of the weight caused by the source with the
     * given index.
     * 
     * The indices start from zero. If the given index is larger than the total number of sources of
     * systematical uncertainty, an exception is thrown. The name of the variation is conventional
     * and does not imply that the returned with is smaller than the nominal one.
     */
    double GetWeightDown(unsigned iSource) const;
    
    /**
     * \brief Returns a vector with the nominal weight and all systematical variations
     * 
     * The vector is always non-empty and contains an odd number of elements. The first element is
     * the nominal weight, it is followed by an "up" variation for the first source of systematical
     * uncertainty, a "down" variation for this sources, and so on.
     */
    std::vector<double> const &GetWeights() const noexcept;
    
protected:
    /**
     * \brief Weights assigned to the current event
     * 
     * The first weight is the nominal one, and it is followed by (optional) systematical
     * variations. Detailed specifications are described in the documentation for method GetWeights.
     */
    std::vector<double> weights;
};

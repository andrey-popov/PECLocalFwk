/**
 * \file WeightBTagInterface.hpp
 * 
 * Defines an abstract base class to evaluate weight of an event due to b-tagging.
 */

#pragma once

#include <PECFwk/core/BTagSFInterface.hpp>
#include <PECFwk/core/Dataset.hpp>

#include <vector>


/**
 * \class WeightBTagInterface
 * \brief An abstract base class to evaluate weight of an event due to b-tagging
 * 
 * The class defines an interface to evaluate event weights to reflect a difference in b-tagging
 * efficiency in data and simulation. A possible implementation in a derived class would operate
 * with classes inherited from BTagEffInterface and BTagSFInterface to access b-tagging efficiencies
 * and scale factors. Nevertheless, other implementations are also possible. For this reason, the
 * class does not contain pointers to these base classes.
 * 
 * The class defines copy and move constructors and assignment operator; it is thread-safe. However,
 * a derived class is not required to be copyable, movable, or thread-safe.
 */
class WeightBTagInterface
{
public:
    /// Supported systematical variations
    enum class Variation
    {
        Nominal,        ///< Nominal weight
        TagRateUp,      ///< Scale factors for b- and c-jets increased
        TagRateDown,    ///< Scale factors for b- and c-jets decreased
        MistagRateUp,   ///< Scale factors for light-flavour and gluon jets increased
        MistagRateDown  ///< Scale factors for light-flavour and gluon jets decreased
    };
    
public:
    /// Default constructor
    WeightBTagInterface() = default;
    
    /// Default copy constructor
    WeightBTagInterface(WeightBTagInterface const &) = default;
    
    /// Default move constructor
    WeightBTagInterface(WeightBTagInterface &&) = default;
    
    /// Default assignment operator
    WeightBTagInterface &operator=(WeightBTagInterface const &) = default;
    
    /// Trivial virtual destructor
    virtual ~WeightBTagInterface();
    
public:
    /**
     * \brief Creates a deep copy of *this
     * 
     * The method is only expected to be executed before the first call to LoadPayload or
     * CalcWeight.
     */
    virtual WeightBTagInterface *Clone() const = 0;
    
    /**
     * \brief Notifies the class that a new dataset is opened
     * 
     * A typical implementation in a derived class will call BTagEffInterface::LoadPayload, although
     * other actions are possible as well. The method must be executed before the first call to
     * CalcWeight for the new dataset.
     * 
     * The default implementation does nothing.
     */
    virtual void LoadPayload(Dataset const &dataset);
    
    /// Calculates event weight
    virtual double CalcWeight(std::vector<Jet> const &jets, Variation var = Variation::Nominal)
     const = 0;

protected:
    /**
     * \brief Reinterprets a variation in terms of BTagSFInterface::Variation enumeration
     * 
     * Provided PDG ID value can be negative. For a mismatched combination, e.g. asking for a
     * TagrateUp variation for a gluon jet, BTagSFInterface::Variation::Nominal is returned.
     * 
     * Although a derived class does not necessarily access a descendant of BTagSFInterface,
     * any reasonable implementation would still need to convert a per-event systematic variation
     * into a per-jet one.
     */
    static BTagSFInterface::Variation TranslateVariation(Variation var, int jetPdgId);
};

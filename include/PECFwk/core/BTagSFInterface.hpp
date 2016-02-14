/**
 * \file BTagSFInterface.hpp
 * 
 * Defines an abstract base class to access scale factors for b-tagging efficiencies.
 */

#pragma once

#include <PECFwk/core/BTagger.hpp>


/**
 * \class BTagSFInterface
 * \brief An abstract base class to access scale factors for b-tagging efficiencies
 * 
 * The class defines an interface for a class to retrieve b-tagging scale factors. A derived class
 * must provide a meaningful implementation for the method GetScaleFactor. The class is not expected
 * to be used with forward jets failing the acceptance defined by the GetMaxPseudorapidity method.
 * 
 * Since it is common to operate with a single working point only, the class provides means to
 * specify default working point and a more convenient accessor to b-tagging scale factor which uses
 * it.
 * 
 * One instance of a derived class is expected to communicate with one thread only. Nevertheless,
 * several instances can share same resources, and special protection might be needed.
 */
class BTagSFInterface
{
public:
    /// Possible variations of scale factors
    enum class Variation
    {
        Nominal,
        Up,
        Down
    };
    
public:
    /**
     * \brief Constructor with no parameters
     * 
     * Sets the default working point to tight.
     */
    BTagSFInterface() noexcept;
    
    /// Default copy constructor
    BTagSFInterface(BTagSFInterface const &) = default;
    
    /// Default move constructor
    BTagSFInterface(BTagSFInterface &&) = default;
    
    /// Default assignment operator
    BTagSFInterface &operator=(BTagSFInterface const &) = default;
    
    /// Trivial virtual destructor
    virtual ~BTagSFInterface() noexcept;
    
public:
    /**
     * \brief Creates a deep copy of *this
     * 
     * The method is intended to create a deep copy of an instance of a derived class. It is only
     * expected to be executed before the first call to GetScaleFactor. A typical implementation in
     * a derived class would be to return a pointer to a new instance created with the copy
     * constructor of the derived class.
     */
    virtual BTagSFInterface *Clone() const = 0;
    
    /**
     * \brief Returns b-tagging scale factor for a given working point and given jet momentum and
     * flavour.
     * 
     * User is not expected to calculate scale factors for jets that fail the pseudorapidity
     * coverage defined by the method GetMaxPseudorapidity. Behaviour of the method is undefined
     * otherwise. The flavour might be negative.
     */
    virtual double GetScaleFactor(BTagger::WorkingPoint wp, Candidate const &jet, int flavour,
     Variation var = Variation::Nominal) const = 0;
    
    /**
     * \brief Returns b-tagging scale factor for a given jet
     * 
     * Internally calls the pure virtual method with the same name.
     */
    double GetScaleFactor(BTagger::WorkingPoint wp, Jet const &jet,
     Variation var = Variation::Nominal) const;
    
    /**
     * \brief Returns b-tagging scale factor for given jet momentum and flavour using the default
     * working point
     * 
     * The method is useful for prescriptions that support only one working point. Internally calls
     * the pure virtual method with the same name.
     */
    double GetScaleFactor(Candidate const &jet, int flavour, Variation var = Variation::Nominal)
     const;
    
    /**
     * \brief Returns b-tagging scale factor for a given jet using the default working point
     * 
     * The method is useful for prescriptions that support only one working point. Internally calls
     * the pure virtual method with the same name.
     */
    double GetScaleFactor(Jet const &jet, Variation var = Variation::Nominal) const;
    
    /// Sets the default working point
    void SetDefaultWorkingPoint(BTagger::WorkingPoint wp);
    
    /**
     * \brief Returns maximal jet pseudorapidity supported for b-tagging scale factors
     * 
     * No scale factors are expected to be available for jets with a larger absolute value of
     * pseudorapidity.
     */
    static double GetMaxPseudorapidity();
    
private:
    /// Default working point for GetEfficiency(Jet const &)
    BTagger::WorkingPoint defaultWP;
    
    /// Maximal supported jet pseudorapidity
    static double const maxPseudorapidity;
};

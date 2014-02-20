/**
 * \file BTagSFInterface.hpp
 * \author Andrey Popov
 * 
 * Defines an abstract base class to access scale factors for b-tagging efficiencies.
 */

#pragma once

#include <BTagger.hpp>


/**
 * \class BTagSFInterface
 * \brief An abstract base class to access scale factors for b-tagging efficiencies
 * 
 * The class defines an interface for a class to retrieve b-tagging scale factors. A derived class
 * must provide a meaningful implementation for the method GetScaleFactor.
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
    BTagSFInterface();
    
    /// Default copy constructor
    BTagSFInterface(BTagSFInterface const &) = default;
    
    /// Default move constructor
    BTagSFInterface(BTagSFInterface &&) = default;
    
    /// Default assignment operator
    BTagSFInterface &operator=(BTagSFInterface const &) = default;
    
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
    
    /// Returns b-tagging scale factor for a given jet
    virtual double GetScaleFactor(BTagger::WorkingPoint wp, Jet const &jet,
     Variation var = Variation::Nominal) const = 0;
    
    /**
     * \brief Returns b-tagging scale factor for a given jet using the default working point
     * 
     * The method is useful for prescriptions that support only one working point.
     */
    double GetScaleFactor(Jet const &jet, Variation var = Variation::Nominal) const;
    
    /// Sets the default working point
    void SetDefaultWorkingPoint(BTagger::WorkingPoint wp);
    
private:
    /// Default working point for GetEfficiency(Jet const &)
    BTagger::WorkingPoint defaultWP;
};

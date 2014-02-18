/**
 * \file BTagEffInterface.hpp
 * \author Andrey Popov
 * 
 * Defines an abstract base class for accessing b-tagging efficiencies.
 */

#pragma once

#include <BTagger.hpp>
#include <Dataset.hpp>


/**
 * \class BTagEffInterface
 * \brief An abstract base class for accessing b-tagging efficiencies
 * 
 * The class defines an interface for a class to retrieve b-tagging efficiencies. A derived class
 * must provide meaningful implementations for methods to read a payload with b-tagging efficiencies
 * and calculate the efficiency for a given jet.
 * 
 * Since it is common to operate with a single working point only, the class provides means to
 * specify default working point and a more convenient accessor to b-tagging efficiency which uses
 * it.
 * 
 * One instance of a derived class is expected to communicate with one thread only. Nevertheless,
 * several instances can share same resources, e.g. a file with b-tagging efficiencies, and special
 * protection might be needed.
 */
class BTagEffInterface
{
public:
    /**
     * \brief Constructor with no parameters
     * 
     * Sets the default working point to tight.
     */
    BTagEffInterface();
    
    /// Default copy constructor
    BTagEffInterface(BTagEffInterface const &) = default;
    
    /// Default move constructor
    BTagEffInterface(BTagEffInterface &&) = default;
    
    /// Default assignment operator
    BTagEffInterface &operator=(BTagEffInterface const &) = default;
    
public:
    /**
     * \brief Creates a deep copy of this
     * 
     * The method is intended to create a deep copy of an instance of a derived class. It is only
     * expected to be called before the first call to LoadPayload or GetEfficiency. Therefore, the
     * typical implementation in a derived class would be to return a pointer to a new instance
     * created with the copy constructor of the derived class.
     */
    virtual BTagEffInterface *Clone() const = 0;
    
    /**
     * \brief Loads b-tagging efficiencies for a given dataset
     * 
     * The method is expected to be executed before the first call to GetEfficiency. It will not
     * not be called frequently and can affort to be slow.
     * 
     * In the default implementation the method does nothing and must be overriden in any meaningful
     * derived class.
     */
    virtual void LoadPayload(Dataset const &dataset);
    
    /// Returns b-tagging efficiency for a given working point and a given jet
    virtual double GetEfficiency(BTagger::WorkingPoint wp, Jet const &jet) const = 0;
    
    /**
     * \brief Returns b-tagging efficiency for a given jet using the default working point
     * 
     * The method is useful for prescriptions that support only one working point.
     */
    virtual double GetEfficiency(Jet const &jet) const;
    
    /// Sets the default working point
    void SetDefaultWorkingPoint(BTagger::WorkingPoint wp);
    
protected:
    /// Default working point for GetEfficiency(Jet const &)
    BTagger::WorkingPoint defaultWP;
};

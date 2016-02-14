/**
 * \file JetCorrectorInterface.hpp
 * 
 * Defines an interface to perform corrections of jet momentum.
 */

#pragma once

#include <PECFwk/core/PhysicsObjects.hpp>
#include <PECFwk/core/SystDefinition.hpp>


/**
 * \class JetCorrectorInterface
 * \brief Defines an interface to perform corrections of jet momentum
 * 
 * An abstract base class to provide an interface for jet correction. Expected to be used to
 * implement jet energy and resolution corrections. Should not change anything but jet
 * four-momentum.
 * 
 * A derived class is not required to be thread-safe.
 */
class JetCorrectorInterface
{
public:
    /**
     * \brief Returns a newly-initialised copy of this
     * 
     * The user must provide a meaningful implementation.
     */
    virtual JetCorrectorInterface *Clone() const noexcept = 0;
    
    /**
     * \brief Initialises the object
     * 
     * Must be executed before the first call to the Correct method. In the default implementation
     * does nothing.
     */
    virtual void Init();
    
    /**
     * \brief Corrects jet four-momentum
     * 
     * The four-momentum is corrected in-place, i.e. the jet is modified. Systematics is evaluated
     * if only the requested variation is relevant to jets; otherwise the third argument is
     * ignored. The user must provide a meaninful implementation for this method.
     * 
     * In addition to the jet's properties, the method takes a value of the mean angular pt density,
     * rho, as it is needed to parametrise L1 JEC.
     */
    virtual void Correct(Jet &jet, double rho, SystVariation syst = SystVariation()) const = 0;
    
    /// A short-cut for the Correct method
    void operator()(Jet &jet, double rho, SystVariation syst = SystVariation()) const;
};

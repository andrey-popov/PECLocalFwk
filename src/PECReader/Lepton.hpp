#pragma once

#include "CandidateWithID.hpp"


namespace pec
{
/**
 * \class Lepton
 * \brief Base class for charged leptons
 */
class Lepton: public CandidateWithID
{
public:
    /// Constructor with no parameters
    Lepton() noexcept;
    
public:
    /// Resets the object to a state right after the default initialisation
    virtual void Reset() override;
    
    /**
     * \brief Sets lepton charge
     * 
     * Checks only the sign of the argument. If it is zero, throws an exception.
     */
    void SetCharge(int charge);
    
    /// Sets relative isolation
    void SetRelIso(float relIso);
    
    /**
     * \brief Returns electric charge of the lepton
     * 
     * The returned value is +1 or -1.
     */
    int Charge() const;
    
    /// Returns relative isolation
    float RelIso() const;
    
private:
    /**
     * \brief Electric charge
     * 
     * True for negative charge (particle), false for positive charge (antiparticle).
     */
    Bool_t charge;
    
    /// Relative isolation
    Float_t relIso;
};
}  // end of namespace pec

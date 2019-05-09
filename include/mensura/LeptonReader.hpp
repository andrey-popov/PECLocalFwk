#pragma once

#include <mensura/ReaderPlugin.hpp>

#include <mensura/PhysicsObjects.hpp>

#include <string>
#include <vector>


/**
 * \class LeptonReader
 * \brief Abstract base class for a reader plugin that provides collections of leptons
 * 
 * Two collections are provided: tight and loose leptons. The latter one is expected to be used
 * to implement veto in lepton counting. The collection of loose leptons includes tight leptons.
 * Leptons of same flavour typically have the same selection on transverse momentum in both tight
 * and loose categories.
 */
class LeptonReader: public ReaderPlugin
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    LeptonReader(std::string const name = "Leptons");
    
    /// Default copy constructor
    LeptonReader(LeptonReader const &) = default;
    
    /// Default move constructor
    LeptonReader(LeptonReader &&) = default;
    
    /// Default assignment operator
    LeptonReader &operator=(LeptonReader const &) = default;
    
    /// Trivial destructor
    virtual ~LeptonReader();
    
public:
    /// Returns collection of tight leptons in the current event
    virtual std::vector<Lepton> const &GetLeptons() const;
    
    /**
     * \brief Returns collection of loose leptons in the current event
     * 
     * This collection includes also all tight leptons.
     */
    virtual std::vector<Lepton> const &GetLooseLeptons() const;
    
protected:
    /// Collection of tight leptons in the current event
    std::vector<Lepton> leptons;
    
    /// Collection of looose leptons in the current event
    std::vector<Lepton> looseLeptons;
};

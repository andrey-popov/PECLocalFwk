#pragma once

#include <PECFwk/core/ReaderPlugin.hpp>

#include <PECFwk/core/GenParticle.hpp>

#include <string>
#include <vector>


/**
 * \class GenParticleReader
 * \brief Abstract base class for a reader plugin that provides collection of generator particles
 * 
 * Typically, only a small subset of all generator-level particles is stored in the input files.
 * This plugin provides an access to this small collection.
 */
class GenParticleReader: public ReaderPlugin
{
public:
    /**
     * \brief Creates a plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    GenParticleReader(std::string const name = "GenParticles");
    
    /// Default copy constructor
    GenParticleReader(GenParticleReader const &) = default;
    
    /// Default move constructor
    GenParticleReader(GenParticleReader &&) = default;
    
    /// Assignment operator is deleted
    GenParticleReader &operator=(GenParticleReader const &) = delete;
    
    /// Trivial destructor
    virtual ~GenParticleReader();
    
public:
    /// Returns collection of selected generator particles in the current event
    virtual std::vector<GenParticle> const &GetParticles() const;
    
protected:
    /// Collection of selected generator particles in the current event
    std::vector<GenParticle> particles;
};

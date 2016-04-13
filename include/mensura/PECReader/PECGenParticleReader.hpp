#pragma once

#include <mensura/core/GenParticleReader.hpp>

#include <mensura/PECReader/GenParticle.hpp>

#include <memory>
#include <string>


class PECInputData;


/**
 * \class PECGenParticleReader
 * \brief Constructs collection of generator-level particles
 * 
 * This plugin reads collection generator-level particles from a PEC file (with the help of a
 * PECInputData plugin) and reconstructs mother-daughter relations.
 */
class PECGenParticleReader: public GenParticleReader
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    PECGenParticleReader(std::string const name = "GenParticles");
    
    /// Copy constructor
    PECGenParticleReader(PECGenParticleReader const &src) noexcept;
    
    /// Default move constructor
    PECGenParticleReader(PECGenParticleReader &&) = default;
    
    /// Assignment operator is deleted
    PECGenParticleReader &operator=(PECGenParticleReader const &) = delete;
    
    /// Trivial destructor
    virtual ~PECGenParticleReader() noexcept;
    
public:
    /**
     * \brief Sets up reading of trees with electrons and muons
     * 
     * Reimplemented from Plugin.
     */
    virtual void BeginRun(Dataset const &) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Plugin.
     */
    virtual Plugin *Clone() const override;
    
private:
    /**
     * \brief Reads electrons and muons from input trees and applied physics selection
     * 
     * Reimplemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Name of the plugin that reads PEC files
    std::string inputDataPluginName;
    
    /// Non-owning pointer to a plugin that reads PEC files
    PECInputData const *inputDataPlugin;
    
    /// Name of the tree containing information about generator-level particles
    std::string treeName;
    
    /// Buffer to read the branch with generator-level particles
    std::vector<pec::GenParticle> bfParticles;
    
    /**
     * \brief An auxiliary pointer to the buffer
     * 
     * Need by ROOT to read the object from a tree.
     */
    decltype(bfParticles) *bfParticlePointer;
};

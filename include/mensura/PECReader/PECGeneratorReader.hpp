#pragma once

#include <mensura/core/GeneratorReader.hpp>

#include <mensura/PECReader/GeneratorInfo.hpp>

#include <vector>


class PECInputData;


/**
 * \class PECGeneratorReader
 * \brief Reads generator-level information about a process from a PEC file
 * 
 * This plugin reads information about generate process, such as LHE-level event weights, input
 * state, etc. Other plugins should be used to access generator-level particles and jets.
 * 
 * This reader relies on PECInputData to get access to the input file. It should only be used with
 * simulated datasets.
 */
class PECGeneratorReader: public GeneratorReader
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    PECGeneratorReader(std::string const name = "Generator");
    
    /// Copy constructor
    PECGeneratorReader(PECGeneratorReader const &src) noexcept;
    
    /// Default move constructor
    PECGeneratorReader(PECGeneratorReader &&) = default;
    
    /// Assignment operator is deleted
    PECGeneratorReader &operator=(PECGeneratorReader const &) = delete;
    
public:
    /**
     * \brief Sets up reading of the tree with generator information
     * 
     * Reimplemented from Plugin.
     */
    virtual void BeginRun(Dataset const &dataset) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Plugin.
     */
    virtual Plugin *Clone() const override;
    
    /**
     * \brief Returns alternative LHE-level weight with the given index
     * 
     * These weights are only available if they have been requested using method RequestAltWeights.
     * Otherwise an exception is thrown.
     * 
     * Reimplemented from GeneratorReader.
     */
    virtual double GetAltWeight(unsigned index) const override;
    
    /**
     * \brief Returns nominal LHE-level weight for the current event
     * 
     * Reimplemented from GeneratorReader.
     */
    virtual double GetNominalWeight() const override;
    
    /**
     * \brief Returns number of available alternative weights
     * 
     * Reimplemented from GeneratorReader.
     */
    virtual unsigned GetNumAltWeights() const override;
    
    /// Returns process ID (as written in LHE file) of the current event
    int GetProcessID() const;
    
    /// Specifies whether alternative LHE-level weights should be read
    void RequestAltWeights(bool on = true);
    
private:
    /**
     * \brief Reads generator information from a PEC file
     * 
     * Reimplemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Name of the plugin that reads PEC files
    std::string inputDataPluginName;
    
    /// Non-owning pointer to a plugin that reads PEC files
    PECInputData const *inputDataPlugin;
    
    /// Flag showing whether alternative weights should be read
    bool readAltWeights;
    
    /// Name of the tree with generator information
    std::string treeName;
    
    /// Buffer to read the only branch of the tree
    pec::GeneratorInfo bfGenerator;
    
    /**
     * \brief An auxiliary pointer to the buffer
     * 
     * Need by ROOT to read the object from a tree.
     */
    decltype(bfGenerator) *bfGeneratorPointer;
};

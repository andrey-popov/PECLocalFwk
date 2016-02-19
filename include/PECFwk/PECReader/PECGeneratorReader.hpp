#pragma once

#include <PECFwk/core/ReaderPlugin.hpp>

#include <PECFwk/PECReader/GeneratorInfo.hpp>


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
 * 
 * Although more information is available in PEC files, for the moment plugin reads only nominal
 * LHE-level event weight and process ID.
 */
class PECGeneratorReader: public ReaderPlugin
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    PECGeneratorReader(std::string const name = "Generator");
    
    /// Default copy constructor
    PECGeneratorReader(PECGeneratorReader const &) = default;
    
    /// Default move constructor
    PECGeneratorReader(PECGeneratorReader &&) = default;
    
    /// Default assignment operator
    PECGeneratorReader &operator=(PECGeneratorReader const &) = default;
    
    /// Trivial destructor
    virtual ~PECGeneratorReader();
    
public:
    /**
     * \brief Sets up reading of the tree with pile-up information
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
    
    /// Returns nominal LHE-level weight of the current event
    double GetNominalWeight() const;
    
    /// Returns process ID (as written in LHE file) of the current event
    int GetProcessID() const;
    
private:
    /**
     * \brief Reads pile-up information for a PEC file
     * 
     * Reimplemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Name of the plugin that reads PEC files
    std::string inputDataPluginName;
    
    /// Non-owning pointer to a plugin that reads PEC files
    PECInputData const *inputDataPlugin;
    
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

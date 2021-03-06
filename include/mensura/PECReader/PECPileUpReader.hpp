#pragma once

#include <mensura/PileUpReader.hpp>


class PECInputData;

namespace pec {
class PileUpInfo;
};


/**
 * \class PECPileUpReader
 * \brief Reads information on pile-up from a PEC file
 * 
 * This plugin relies on PECInputData to get access to the input file.
 */
class PECPileUpReader: public PileUpReader
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    PECPileUpReader(std::string const name = "PileUp");
    
    /// Copy constructor
    PECPileUpReader(PECPileUpReader const &src) noexcept;
    
    /// Default move constructor
    PECPileUpReader(PECPileUpReader &&) = default;
    
    /// Assignment operator is deleted
    PECPileUpReader &operator=(PECPileUpReader const &) = delete;
    
    /// Trivial destructor
    virtual ~PECPileUpReader() noexcept;
    
public:
    /**
     * \brief Sets up reading of the tree with pile-up information
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
    
    /// Name of the tree containing pile-up information
    std::string treeName;
    
    /**
     * \brief Pointer to buffer to read the only branch of the tree
     *
     * The buffer is allocated by TBranch.
     */
    pec::PileUpInfo *bfPileUpInfoP;
};

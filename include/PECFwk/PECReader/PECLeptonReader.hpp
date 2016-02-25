#pragma once

#include <PECFwk/core/LeptonReader.hpp>

#include <PECFwk/PECReader/Electron.hpp>
#include <PECFwk/PECReader/Muon.hpp>

#include <memory>
#include <string>


class PECInputData;


/**
 * \class PECLeptonReader
 * \brief Constructs collections of analysis-level electrons and muons
 * 
 * This plugin reads collections of electrons and muons from a PEC file (with the help of a
 * PECInputData plugin), translates them to the standard class Lepton used by the framework, and
 * applies required filtering to construct collections of loose and tight leptons.
 */
class PECLeptonReader: public LeptonReader
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    PECLeptonReader(std::string const name = "Leptons");
    
    /// Copy constructor
    PECLeptonReader(PECLeptonReader const &src) noexcept;
    
    /// Default move constructor
    PECLeptonReader(PECLeptonReader &&) = default;
    
    /// Assignment operator is deleted
    PECLeptonReader &operator=(PECLeptonReader const &) = delete;
    
    /// Trivial destructor
    virtual ~PECLeptonReader() noexcept;
    
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
    
    /// Name of the tree containing information about electrons
    std::string electronTreeName;
    
    /// Buffer to read the branch with electrons
    std::vector<pec::Electron> bfElectrons;
    
    /**
     * \brief An auxiliary pointer to electron buffer
     * 
     * Need by ROOT to read the object from a tree.
     */
    decltype(bfElectrons) *bfElectronPointer;
    
    /// Name of the tree containing information about muons
    std::string muonTreeName;
    
    /// Buffer to read the branch with muons
    std::vector<pec::Muon> bfMuons;
    
    /**
     * \brief An auxiliary pointer to muon buffer
     * 
     * Need by ROOT to read the object from a tree.
     */
    decltype(bfMuons) *bfMuonPointer;
};

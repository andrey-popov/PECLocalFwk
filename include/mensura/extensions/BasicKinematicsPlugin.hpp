#pragma once

#include <mensura/core/AnalysisPlugin.hpp>

#include <TTree.h>

#include <string>


class LeptonReader;
class JetMETReader;
class PileUpReader;
class TFileService;


/**
 * \class BasicKinematicsPlugin
 * \brief A plugin that evaluates and stores basic kinematical variables
 * 
 * This plugin represents an example of producing custom trees with some representative
 * observables. It uses a TFileService with a default name "TFileService" to create the output tree.
 * Inputs are access with dedicated readers with default names "Leptons", "JetMET", and "PileUp".
 */
class BasicKinematicsPlugin: public AnalysisPlugin
{
public:
    /// Constructor
    BasicKinematicsPlugin(std::string const name = "BasicKinematics");
    
    /// Default move constructor
    BasicKinematicsPlugin(BasicKinematicsPlugin &&) = default;
    
    /// Assignment operator is deleted
    BasicKinematicsPlugin &operator=(BasicKinematicsPlugin const &) = delete;
    
    /// Trivial destructor
    virtual ~BasicKinematicsPlugin() noexcept;
    
private:
    /**
     * \brief Copy constructor that produces a newly initialized clone
     * 
     * Behaviour of this copy constructor is appropriate only before processing of the first
     * dataset starts since attributes that are created in BeginRun are not copy. For this reason
     * the copy constructor must not be used in a generic case and made private to prevent this.
     */
    BasicKinematicsPlugin(BasicKinematicsPlugin const &src);
    
public:
    /**
     * \brief Creates a new output file and sets up a tree
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
    
private:
    /**
     * \brief Computes representative observables for the current event and fills the tree
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Name of TFileService
    std::string fileServiceName;
    
    /// Non-owning pointer to TFileService
    TFileService const *fileService;
    
    /// Name of the plugin that produces leptons
    std::string leptonPluginName;
    
    /// Non-owning pointer to the plugin that produces leptons
    LeptonReader const *leptonPlugin;
    
    /// Name of the plugin that produces jets and MET
    std::string jetmetPluginName;
    
    /// Non-owning pointer to the plugin that produces jets and MET
    JetMETReader const *jetmetPlugin;
    
    /// Name of the plugin that reads information about pile-up
    std::string puPluginName;
    
    /// Non-owning pointer to the plugin that reads information about pile-up
    PileUpReader const *puPlugin;
    
    /// Non-owning pointer to the output tree
    TTree *tree;
    
    // Output buffers
    Float_t Pt_Lep, Eta_Lep;
    Float_t Pt_J1, Eta_J1, Pt_J2, Eta_J2;
    Float_t M_J1J2, DR_J1J2;
    Float_t MET, MtW;
    Int_t nPV;
};

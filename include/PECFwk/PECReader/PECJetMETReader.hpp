#pragma once

#include <PECFwk/core/JetMETReader.hpp>

#include <PECFwk/PECReader/Candidate.hpp>
#include <PECFwk/PECReader/Jet.hpp>


class PECInputData;


/**
 * \class PECJetMETReader
 * \brief Provides reconstructed jets and MET
 * 
 * 
 */
class PECJetMETReader: public JetMETReader
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    PECJetMETReader(std::string const name = "JetMET");
    
    /// Default copy constructor
    PECJetMETReader(PECJetMETReader const &) = default;
    
    /// Default move constructor
    PECJetMETReader(PECJetMETReader &&) = default;
    
    /// Default assignment operator
    PECJetMETReader &operator=(PECJetMETReader const &) = default;
    
    /// Trivial destructor
    virtual ~PECJetMETReader() noexcept;
    
public:
    /**
     * \brief Sets up reading of a tree containing jets and MET
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
     * \brief Reads jets and MET from the input tree
     * 
     * Reimplemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Name of the plugin that reads PEC files
    std::string inputDataPluginName;
    
    /// Non-owning pointer to a plugin that reads PEC files
    PECInputData const *inputDataPlugin;
    
    /// Name of the tree containing information about jets and MET
    std::string treeName;
    
    /// Buffer to read the branch with jets
    std::vector<pec::Jet> bfJets;
    
    /**
     * \brief An auxiliary pointer to jet buffer
     * 
     * Need by ROOT to read the object from a tree.
     */
    decltype(bfJets) *bfJetPointer;
    
    /// Buffer to read the branch with METs
    std::vector<pec::Candidate> bfMETs;
    
    /**
     * \brief An auxiliary pointer to MET buffer
     * 
     * Need by ROOT to read the object from a tree.
     */
    decltype(bfMETs) *bfMETPointer;
};

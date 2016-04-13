#pragma once

#include <mensura/core/GenJetMETReader.hpp>

#include <mensura/PECReader/Candidate.hpp>
#include <mensura/PECReader/GenJet.hpp>


class PECInputData;


/**
 * \class PECGenJetMETReader
 * \brief Provides generator-level jets and MET
 * 
 * This plugin reads generator-level jets and MET from a PEC file, with the help of a PECInputData
 * plugin, and translates them to standard classes used by the framework. User can specify a
 * kinematic selection to be applied to jets, using method SetSelection.
 */
class PECGenJetMETReader: public GenJetMETReader
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    PECGenJetMETReader(std::string const name = "GenJetMET");
    
    /// Copy constructor
    PECGenJetMETReader(PECGenJetMETReader const &src) noexcept;
    
    /// Default move constructor
    PECGenJetMETReader(PECGenJetMETReader &&) = default;
    
    /// Assignment operator is deleted
    PECGenJetMETReader &operator=(PECGenJetMETReader const &) = delete;
    
    /// Trivial destructor
    virtual ~PECGenJetMETReader() noexcept;
    
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
    
    /**
     * \brief Returns radius parameter used in the jet clustering algorithm
     * 
     * The radius is hard-coded in the current implementation, but it will be made configurable in
     * future if jets with larger radii are added.
     * 
     * Implemented from GenJetMETReader.
     */
    virtual double GetJetRadius() const override;
    
    /// Specifiy desired selection on jets
    void SetSelection(double minPt, double maxAbsEta);
    
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
    std::vector<pec::GenJet> bfJets;
    
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
    
    /// Minimal allowed transverse momentum
    double minPt;
    
    /// Maximal allowed absolute value of pseudorapidity
    double maxAbsEta;
};

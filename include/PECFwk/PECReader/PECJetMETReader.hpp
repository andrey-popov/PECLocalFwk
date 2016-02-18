#pragma once

#include <PECFwk/core/JetMETReader.hpp>

#include <PECFwk/core/LeptonReader.hpp>
#include <PECFwk/PECReader/Candidate.hpp>
#include <PECFwk/PECReader/Jet.hpp>


class PECInputData;


/**
 * \class PECJetMETReader
 * \brief Provides reconstructed jets and MET
 * 
 * This plugin reads reconstructed jets and MET from a PEC file, with the help of a PECInputData
 * plugin, and translates them to standard classes used by the framework. User can specify a
 * kinematic selection to be applied to jets, using method SetSelection. By default, jets are
 * cleaned against tight leptons produced by a LeptonReader with name "Leptons". This behaviour
 * can be configured with method ConfigureLeptonCleaning.
 * 
 * Currently the plugin is not able to reapply JEC, does not perform matching to GEN-level jets,
 * and neglects any systematic variations.
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
     * \brief Changes parameters of jet-lepton cleaning
     * 
     * The first argument is the name of a LeptonReader, which must precede this plugin in the
     * path. The second argument is the minimal allowed separation between a jet and a lepton in
     * the (eta, phi) metric. Each jets is checked against all tight leptons produced by the
     * LeptonReader, and if the separation from a lepton is less than dR, the jet is dropped.
     * 
     * The cleaning is enabled by default, with parameters specified in the prototype. It can be
     * disabled by providing an emptry name ("") for the LeptonReader.
     */
    void ConfigureLeptonCleaning(std::string const leptonPluginName = "Leptons", double dR = 0.3);
    
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
    
    /// Minimal allowed transverse momentum
    double minPt;
    
    /// Maximal allowed absolute value of pseudorapidity
    double maxAbsEta;
    
    /**
     * \brief Name of the plugin that produces leptons
     * 
     * If name is an empty string, no cleaning against leptons will be performed
     */
    std::string leptonPluginName;
    
    /// Non-owning pointer to a plugin that produces leptons
    LeptonReader const *leptonPlugin;
    
    /**
     * \brief Minimal squared dR distance to leptons
     * 
     * Exploited in jet cleaning.
     */
    double leptonDR2;
};

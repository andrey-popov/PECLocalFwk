#pragma once

#include <mensura/core/JetMETReader.hpp>

#include <mensura/core/LeptonReader.hpp>
#include <mensura/core/GenJetMETReader.hpp>
#include <mensura/core/SystService.hpp>

#include <mensura/external/JERC/JetResolution.hpp>

#include <mensura/PECReader/Candidate.hpp>
#include <mensura/PECReader/Jet.hpp>

#include <memory>


class PileUpReader;
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
 * If the name of a plugin that reads generator-level jets is provided with the help of the method
 * SetGenJetReader, angular matching to them is performed. The maximal allowed angular distance for
 * matching is set to half of the radius parameter of reconstructed jets. User can additionally
 * impose a cut on the difference between pt of the two jets via method SetGenPtMatching.
 * 
 * Systematic variations in JEC, JER, or "unclustered MET" are applied as requested by a
 * SystService with a default name "Systematics". The service is optional; if it is not defined,
 * variations are not performed.
 */
class PECJetMETReader: public JetMETReader
{
private:
    /// Supported systematic variations
    enum class SystType
    {
        None,
        JEC,
        JER,
        METUncl
    };
    
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    PECJetMETReader(std::string const name = "JetMET");
    
    /// Copy constructor
    PECJetMETReader(PECJetMETReader const &src) noexcept;
    
    /// Default move constructor
    PECJetMETReader(PECJetMETReader &&) = default;
    
    /// Assignment operator is deleted
    PECJetMETReader &operator=(PECJetMETReader const &) = delete;
    
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
     * \brief Changes parameters of jet-lepton cleaning
     * 
     * The first argument is the name of a LeptonReader, which must precede this plugin in the
     * path. The second argument is the minimal allowed separation between a jet and a lepton in
     * the (eta, phi) metric. Each jets is checked against all tight leptons produced by the
     * LeptonReader, and if the separation from a lepton is less than dR, the jet is dropped.
     * 
     * The cleaning is enabled by default, with the lepton reader called "Leptons" and dR equal to
     * the jet radius. It can be disabled by providing an empty name ("") for the lepton reader.
     */
    void ConfigureLeptonCleaning(std::string const leptonPluginName, double dR);
    
    /// A short-cut for the above method that uses jet radius as the minimal allowed separation
    void ConfigureLeptonCleaning(std::string const leptonPluginName = "Leptons");
    
    /**
     * \brief Returns radius parameter used in the jet clustering algorithm
     * 
     * The radius is hard-coded in the current implementation, but it will be made configurable in
     * future if jets with larger radii are added.
     * 
     * Implemented from JetMETReader.
     */
    virtual double GetJetRadius() const override;
    
    /// Requests reading of raw MET
    void ReadRawMET(bool enable = true);
    
    /**
     * \brief Specifies whether jet ID selection should be applied or not
     * 
     * The selection is applied or not depending on the value of the given flag; by default, the
     * plugin is configured to apply it. If the selection is applied, jets that fail loose ID are
     * rejected. Otherwise all jets are written to the output collection, and for each jet a
     * UserInt with label "ID" is added whose value is 1 or 0 depending on whether the jet passes
     * the ID selection or not.
     */
    void SetApplyJetID(bool applyJetID);
    
    /// Specifies name of the plugin that provides generator-level jets
    void SetGenJetReader(std::string const name = "GenJetMET");
    
    /**
     * \brief Add a condition on difference in pt for the matching to generator-level jets
     * 
     * If this method is called, the matching to generator-level jets will be performed based not
     * only on the angular distance but requiring additionally that the difference in pt between
     * the two jets is compatible with the pt resolution in simulation.
     * 
     * The pt resultion is described in file jerFile, which must follow the standard format adopted
     * in the JERC group. The path to the file is resolved using the FileInPath service under a
     * subdirectory "JERC". The factor jerPtFactor is applied to the resolution before the
     * comparison.
     * 
     * If the pt matching has been requested, this plugin will also attempt to read pileup
     * information from the dedicated plugin.
     */
    void SetGenPtMatching(std::string const &jerFile, double jerPtFactor = 3.);
    
    /// Specifies desired selection on jets
    void SetSelection(double minPt, double maxAbsEta);
    
private:
    /**
     * \brief Reads jets and MET from the input tree
     * 
     * Reimplemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Name of a plugin that reads PEC files
    std::string inputDataPluginName;
    
    /// Non-owning pointer to a plugin that reads PEC files
    PECInputData const *inputDataPlugin;
    
    /// Name of a service that reports requested systematics
    std::string systServiceName;
    
    /// Name of the tree containing information about jets and MET
    std::string treeName;
    
    /// Buffer to read the branch with jets
    std::vector<pec::Jet> bfJets;
    
    /**
     * \brief An auxiliary pointer to jet buffer
     * 
     * Needed by ROOT to read the object from a tree.
     */
    decltype(bfJets) *bfJetPointer;
    
    /// Buffers to read the branches with METs
    std::vector<pec::Candidate> bfMETs, bfUncorrMETs;
    
    /**
     * \brief Auxiliary pointers to MET buffers
     * 
     * Needed by ROOT to read the object from a tree.
     */
    decltype(bfMETs) *bfMETPointer, *bfUncorrMETPointer;
    
    /// Minimal allowed transverse momentum
    double minPt;
    
    /// Maximal allowed absolute value of pseudorapidity
    double maxAbsEta;
    
    /// Specifies whether raw MET should be read
    bool readRawMET;
    
    /// Specifies whether selection on jet ID should be applied
    bool applyJetID;
    
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
    
    /**
     * \brief Name of the plugin that produces generator-level jets
     * 
     * The name can be empty. In this case no matching to generator-level jets is performed.
     */
    std::string genJetPluginName;
    
    /// Non-owning pointer to a plugin that produces generator-level jets
    GenJetMETReader const *genJetPlugin;
    
    /// Name of the plugin that provides information about pileup
    std::string puPluginName;
    
    /**
     * \brief Non-owning pointer to a plugin that produces information about pileup
     * 
     * Only used when jet resolution needs to be accessed and can be null in other cases.
     */
    PileUpReader const *puPlugin;
    
    /**
     * \brief Fully-qualified path to file with jet pt resolution in simulation
     * 
     * This string is empty if the jet matching based on pt has not been requested.
     */
    std::string jerFilePath;
    
    /// Factor used to define the cut for the jet pt matching
    double jerPtFactor;
    
    /**
     * \brief An object that provides jet pt resolution in simulation
     * 
     * Uninitialized if the jet matching based on pt has not been requested.
     */
    std::unique_ptr<JME::JetResolution> jerProvider;
    
    /// Type of requested systematical variation
    SystType systType;
    
    /**
     * \brief Requested direction of a systematical variation
     * 
     * Allowed values are 0, -1, +1.
     */
    int systDirection;
};

#pragma once

#include <mensura/core/JetMETReader.hpp>

#include <mensura/core/SystService.hpp>
#include <mensura/extensions/JetCorrectorService.hpp>


class PileUpReader;


/**
 * \class JetMETUpdate
 * \brief A plugin that applies energy corrections to jets and propagate them into MET
 * 
 * 
 */
class JetMETUpdate: public JetMETReader
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    JetMETUpdate(std::string const name = "JetMET");
    
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
     * Implemented from JetMETReader.
     */
    virtual double GetJetRadius() const override;
    
    /**
     */
    void SetJetCorrection(std::string const &jetCorrServiceName);
    
    /**
     */
    void SetJetCorrectionForMET(std::string const &fullNew, std::string const &l1New,
      std::string const &fullOrig, std::string const &l1Orig);
    
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
    /// Non-owning pointer to and name of a plugin that reads jets and MET
    JetMETReader const *jetmetPlugin;
    std::string jetmetPluginName;
    
    /// Non-owning pointer to and name of a plugin that reads information about pile-up
    PileUpReader const *puPlugin;
    std::string puPluginName;
    
    /// Name of a service that reports requested systematics
    std::string systServiceName;
    
    /**
     * \brief Non-owning pointer to and name of a service to recorrect jets
     * 
     * Might be uninitialized.
     */
    JetCorrectorService const *jetCorrForJets;
    std::string jetCorrForJetsName;
    
    /**
     * \brief Non-owning pointers to and names of services to recorrect jets for T1 MET
     * 
     * Services for a full and L1 only corrections. Might be uninitialized.
     */
    JetCorrectorService const *jetCorrForMETFull, *jetCorrForMETL1;
    std::string jetCorrForMETFullName, jetCorrForMETL1Name;
    
    /**
     * \brief Non-owning pointers to and names of services to undo T1 MET corrections
     * 
     * Services for a full and L1 only corrections as used in construction of T1-corrected MET.
     * Might be uninitialized.
     */
    JetCorrectorService const *jetCorrForMETOrigFull, *jetCorrForMETOrigL1;
    std::string jetCorrForMETOrigFullName, jetCorrForMETOrigL1Name;
    
    /// Minimal allowed transverse momentum
    double minPt;
    
    /// Maximal allowed absolute value of pseudorapidity
    double maxAbsEta;
    
    /// Minimal transverse momentum for jets to be considered in T1 MET corrections
    double minPtForT1;
    
    /// Type of requested systematical variation
    JetCorrectorService::SystType systType;
    
    /// Requested direction of a systematical variation
    SystService::VarDirection systDirection;
};

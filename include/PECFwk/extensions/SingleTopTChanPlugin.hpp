/**
 * \file SingleTopTChanPlugin.hpp
 * 
 * The module defines a class to calculate a set of variables that are used in single-top t-channel
 * analysis.
 */

#pragma once

#include <PECFwk/core/AnalysisPlugin.hpp>

#include <PECFwk/core/PECReaderPlugin.hpp>
#include <PECFwk/core/BTagger.hpp>

#include <TFile.h>
#include <TTree.h>

#include <string>


/**
 * \class SingleTopTChanPlugin
 * \brief Calculates and stores variables that are used in single-top t-channel analysis
 * 
 * The class is expected to serve as an illustration rather than to be used in a real-life analysis.
 */
class SingleTopTChanPlugin: public AnalysisPlugin
{
public:
    /// Constructor
    SingleTopTChanPlugin(std::string const &outDirectory, BTagger const &bTagger);

public:
    /**
     * \brief Creates a newly-initialized copy
     * 
     * Consult documentation of the overriden method for details.
     */
    Plugin *Clone() const;
    
    /**
     * \brief Notifies this that a dataset has been opened
     * 
     * Consult documentation of the overriden method for details.
     */
    void BeginRun(Dataset const &dataset);
    
    /**
     * \brief Notifies this that a dataset has been closed
     * 
     * Consult documentation of the overriden method for details.
     */
    void EndRun();
    
private:
    /**
     * \brief Processes the current event
     * 
     * Consult documentation of the overriden method for details.
     */
    bool ProcessEvent();

private:
    /// Pointer to PECReaderPlugin
    PECReaderPlugin const *reader;
    
    /// An object to perform b-tagging
    BTagger const &bTagger;
    
    /// Directory to store output files
    std::string outDirectory;
    
    /// Current output file
    TFile *file;
    
    /// Current output tree
    TTree *tree;
    
    // Output buffers
    ULong64_t eventNumber, runNumber, lumiSection;
    
    Float_t Pt_Lep, Eta_Lep;
    Float_t MET, MtW;
    Float_t Phi_MET;
    
    Float_t Pt_J1, Eta_J1, Pt_J2, Eta_J2;
    Float_t Pt_LJ, Eta_LJ;
    Float_t Pt_BJ1;
    
    Float_t M_J1J2, DR_J1J2, Pt_J1J2;
    
    Float_t Ht;
    Float_t M_JW;
    
    Float_t Mtop_BJ1;
    Float_t Cos_LepLJ_BJ1;
    
    Float_t Sphericity;
    
    Int_t nPV;
    Float_t weight;
};

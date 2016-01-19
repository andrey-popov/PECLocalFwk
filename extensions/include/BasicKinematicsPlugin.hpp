/**
 * \file BasicKinematicsPlugin.hpp
 * 
 * The module defines a plugin to store basic kinematical information.
 */

#pragma once

#include <AnalysisPlugin.hpp>
#include <PECReaderPlugin.hpp>

#include <TFile.h>
#include <TTree.h>

#include <string>


/**
 * \class BasicKinematicsPlugin
 * \brief A plugin to store basic kinematical information
 * 
 * The class is provided more as an illustration of the plugin concept rather than is expected to
 * be used in a real-life analysis.
 */
class BasicKinematicsPlugin: public AnalysisPlugin
{
public:
    /// Constructor
    BasicKinematicsPlugin(std::string const &outDirectory);

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
    
    /// Directory to store output files
    std::string outDirectory;
    
    /// Current output file
    TFile *file;
    
    /// Current output tree
    TTree *tree;
    
    // Output buffers
    Float_t Pt_Lep, Eta_Lep;
    Float_t Pt_J1, Eta_J1, Pt_J2, Eta_J2;
    Float_t M_J1J2, DR_J1J2;
    Float_t MET, MtW;
    Int_t nPV;
    Float_t weight;
};

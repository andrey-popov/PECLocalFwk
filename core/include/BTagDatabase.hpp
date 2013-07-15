/**
 * \file BTagDatabase.hpp
 * \author Andrey Popov
 * 
 * Provides means to access b-tagging efficiencies and scale factors.
 */

#pragma once

#include <BTagger.hpp>
#include <Dataset.hpp>

#include <TFile.h>
#include <TH2.h>

#include <string>
#include <vector>
#include <memory>


/**
 * \class BTagDatabase
 * \brief Provides an interface to access b-tagging efficiencies and scale factors
 * 
 * The class provides an access to b-tagging MC efficiencies and data/MC scale factors. The scale
 * factors are provided in [1] and are implemented via automatically generated code. The code was
 * created with the help of script [2].
 * [1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagPOG#2012_Data_and_MC_Moriond13_presc
 * [2] /afs/cern.ch/user/a/aapopov/workspace/tHq/2012Alpha/2013.04.15_BTagSF/generateSFCode.py
 * 
 * The class is copyable.
 */
class BTagDatabase
{
    public:
        /// Supported payloads of scale factors for b-jets
        enum class BJetPayload
        {
            TTBar,  ///< Scale factors from ttbar data
            MuJetTTBar  ///< Scale factors from both mu+jets and ttbar data
        };
        
        /// Possible variations of scale factors
        enum class SFVar
        {
            Central,
            Up,
            Down
        };
    
    public:
        /**
         * \brief Constructor
         * 
         * The arguments are an instance of class BTagger, which is used to access the b-tagging
         * algorithm and the working point, and a payload for b-jets. Though for the time being only
         * one payload is implemented.
         */
        BTagDatabase(BTagger const &bTagger, BJetPayload bJetPayload_ = BJetPayload::MuJetTTBar);
        
        /// Copy constructor
        BTagDatabase(BTagDatabase const &src);
        
        /// Assignment operator is disabled
        BTagDatabase& operator=(BTagDatabase const &) = delete;
        
        /// Descructor
        ~BTagDatabase();
    
    public:
        /**
         * \brief Informs the class of the current dataset
         * 
         * This information is needed to choose a payload for efficiencies, for example. It is
         * expected to be updated rarely. The method must be called before accessing any
         * information from the database.
         */
        void SetDataset(Dataset const &dataset);
        
        /// Returns b-tagging efficiency for a given jet
        double GetEfficiency(Jet const &jet) const;
        
        /// Returns b-tagging scale factor for a given jet
        double GetScaleFactor(Jet const &jet, SFVar var = SFVar::Central) const;
    
    private:
        /// Reads histogram with b-tagging efficiencies for the current dataset
        void SetEfficiencies();
        
        // The code below was generated automatically
        double GetSFTagCSVL(double pt) const;
        double GetSFTagCSVM(double pt) const;
        double GetSFTagCSVT(double pt) const;
        double GetSFTagTCHPT(double pt) const;
        double GetSFTagJPL(double pt) const;
        double GetSFTagJPM(double pt) const;
        double GetSFTagJPT(double pt) const;

        double GetSFMistagCSVLMean(double pt, double absEta) const;
        double GetSFMistagCSVLMin(double pt, double absEta) const;
        double GetSFMistagCSVLMax(double pt, double absEta) const;
        double GetSFMistagCSVMMean(double pt, double absEta) const;
        double GetSFMistagCSVMMin(double pt, double absEta) const;
        double GetSFMistagCSVMMax(double pt, double absEta) const;
        double GetSFMistagCSVTMean(double pt, double absEta) const;
        double GetSFMistagCSVTMin(double pt, double absEta) const;
        double GetSFMistagCSVTMax(double pt, double absEta) const;
        double GetSFMistagTCHPTMean(double pt, double absEta) const;
        double GetSFMistagTCHPTMin(double pt, double absEta) const;
        double GetSFMistagTCHPTMax(double pt, double absEta) const;
        double GetSFMistagJPLMean(double pt, double absEta) const;
        double GetSFMistagJPLMin(double pt, double absEta) const;
        double GetSFMistagJPLMax(double pt, double absEta) const;
        double GetSFMistagJPMMean(double pt, double absEta) const;
        double GetSFMistagJPMMin(double pt, double absEta) const;
        double GetSFMistagJPMMax(double pt, double absEta) const;
        double GetSFMistagJPTMean(double pt, double absEta) const;
        double GetSFMistagJPTMin(double pt, double absEta) const;
        double GetSFMistagJPTMax(double pt, double absEta) const;
        // End of automatically generated code
      
    private:
        BTagger::Algorithm const algo;  ///< Chosen b-tagging algorithm
        BTagger::WorkingPoint const workingPoint;  ///< Chosen b-tagging working point
        BJetPayload bJetPayload;  ///< Payload of the scale factors for b-jets to be used
        
        /// Pointer to the requested method to access the scale factors for b-jets
        double (BTagDatabase::*tagScaleFactorMethod)(double) const;
        
        /// Vector with uncertainties of the scale factors for b-jets
        std::vector<double> tagUncertainties;
        
        /// Pointer to the requested method to access the nominal mistag scale factor
        double (BTagDatabase::*mistagScaleFactorMethodMean)(double, double) const;
        
        /// Pointer to the requested method to access the minimal mistag scale factor
        double (BTagDatabase::*mistagScaleFactorMethodMin)(double, double) const;
        
        /// Pointer to the requested method to access the maximal mistag scale factor
        double (BTagDatabase::*mistagScaleFactorMethodMax)(double, double) const;
        
        /// Pseudorapidity to define an outer region (it has a lower upper boundary in pt)
        double mistagOuterRegion;
        
        /// ROOT file containing b-tagging efficiencies (shared among copies of an object)
        std::shared_ptr<TFile> effFile;
        mutable TH2 *histEffB, *histEffC, *histEffL, *histEffG;  ///< Histograms with efficiencies
};

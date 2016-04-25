#pragma once

#include <mensura/extensions/EventWeightPlugin.hpp>

#include <TFile.h>
#include <TH1.h>

#include <memory>
#include <string>


class PileUpReader;


/**
 * \class PileUpWeight
 * \brief Plugin that implements reweighting for additional pp interactions ("pile-up")
 * 
 * This plugin performs reweighting on pile-up based on the expected number of pile-up
 * interactions, or the "true" simulated pile-up. The idea follows the official recipe [1].
 * 
 * Inputs needed for the reweighting are the target (data) distribution of pile-up (normally it is
 * constructed with the help of script pileupCalc.py [2]), the pile-up profile used in simulation,
 * and the desired systematic variation as described in [3]. It is possible to provide individual
 * simulated profile for every process, although usually a common one would suffice.
 * 
 * This plugin exploits a PileUpReader with the default name "PileUp".
 * 
 * [1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupMCReweightingUtilities
 * [2] https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData?rev=22
 * [3] https://twiki.cern.ch/twiki/bin/view/CMS/PileupSystematicErrors
 */
class PileUpWeight: public EventWeightPlugin
{
public:
    /**
     * \brief Creates a plugin with the given name
     * 
     * Input arguments specify names of files containing distributions of "true" number of pile-up
     * interactions in data and simulation. By default, provided file names are resolved with
     * respect to directory data/PileUp/. The data file must contain a histogram named "pileup"
     * that describes the distribution. The file with profiles in simulation may contain individual
     * distributions for some or all processes (named according to labels returned by method
     * Dataset::GetSourceDatasetID), and, in addition to them, it must include the default
     * distribution named "nominal". Input histograms with arbitrary normalization and binning
     * (including variable one) are supported.
     * 
     * The last argument sets desired systematical variation as defined in [1].
     * [1] https://twiki.cern.ch/twiki/bin/view/CMS/PileupSystematicErrors
     */
    PileUpWeight(std::string const &name, std::string const &dataPUFileName,
      std::string const &mcPUFileName, double systError);
    
    /// A short-cut for the above version with a default name "PileUpWeight"
    PileUpWeight(std::string const &dataPUFileName, std::string const &mcPUFileName,
      double systError);
    
    /// Default copy constructor
    PileUpWeight(PileUpWeight const &) = default;
    
    /// Default move constructor
    PileUpWeight(PileUpWeight &&) = default;
    
    /// Assignment operator is deleted
    PileUpWeight &operator=(PileUpWeight const &) = delete;
    
    /// Trivial destructor
    virtual ~PileUpWeight() noexcept;

public:
    /**
     * \brief Saves pointers to required plugins and loads simulated pile-up profile for the
     * current dataset
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
     * \brief Calculates weights for the current event
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
    /// Reads target (data) pile-up distribution from the given file
    void ReadTargetDistribution(std::string const &dataPUFileName);

private:
    /// Name of the plugin that reads information about pile-up
    std::string puPluginName;
    
    /// Non-owning pointer to the plugin that reads information about pile-up
    PileUpReader const *puPlugin;
    
    /// Target pile-up distribution in data
    std::shared_ptr<TH1> dataPUHist;
    
    /// File with per-dataset distributions of expected pile-up in simulation
    std::shared_ptr<TFile> mcPUFile;
    
    /// Distribution of expected pile-up used in generation of the current MC dataset
    std::shared_ptr<TH1> mcPUHist;
    
    /// Rescaling of the target distribution to estimate systematical uncertainty
    double const systError;
};

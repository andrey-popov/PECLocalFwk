#pragma once

#include <PECFwk/core/AnalysisPlugin.hpp>

#include <TFile.h>
#include <TH1.h>

#include <memory>
#include <string>


class PileUpReader;


/**
 * \class WeightPileUp
 * \brief Plugin that implements reweighting for additional pp interactions ("pile-up")
 * 
 * This plugin performs reweighting on pile-up based on the expected number of pile-up
 * interactions, or "true" simulated pile-up. The idea follows the official recipe [1].
 * 
 * The input parameters for the algorithm are a file with the target (data) distribution of pile-up
 * (normally this file is created by script pileupCalc.py [2]) and an amount of scaling of the
 * number of pile-up interactions that is used to account for the systematical variation (as
 * described in [3]).
 * 
 * In past there were problems with the random-number engine exploited to admix pile-up to MC,
 * which made the actual pile-up distribution in MC deviate from the nominal one. This plugin is
 * capable of handling such cases, allowing user to supply a ROOT file with actual pile-up MC
 * histograms, which are then used instead of nominal one.
 * 
 * The nominal MC pile-up distribution used by default is S10 (adopted in Summer12 campaign).
 * 
 * This plugin exploits a PileUpReader with the default name "PileUp".
 * 
 * [1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupMCReweightingUtilities
 * [2] https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData?rev=22
 * [3] https://twiki.cern.ch/twiki/bin/view/CMS/PileupSystematicErrors
 */
class WeightPileUp: public AnalysisPlugin
{
public:
    /// An auxiliary structure to aggregate central weight and its systematical variations
    struct Weights
    {
        /// Central weight
        double central;
        
        /// Up variation
        double up;
        
        /// Down variation
        double down;
    };
    
public:
    /**
     * \brief Constructor
     * 
     * The first argument is the name of a file containing "true" data distribution over number
     * of pile-up interactions. The file must contain a histogram named "pileup" that describes
     * the distribution. No assumption about binning of the histogram is made (the code works
     * with any one). The second parameter is the name of a file with MC distributions over the
     * "true" number of pile-up interactions before any event selection (it might differ from
     * the nominal distribution due to a bug in random-number engine). The both file names are
     * resolved w.r.t. directory $PEC_FWK_INSTALL/data/PileUp/. The last parameter is a
     * desired systematical variation as defined in [1].
     * [1] https://twiki.cern.ch/twiki/bin/view/CMS/PileupSystematicErrors
     */
    WeightPileUp(std::string const &name, std::string const &dataPUFileName,
      std::string const &mcPUFileName, double systError);
    
    /**
     * \brief Constuctor
     * 
     * See documentation for WeightPileUp(string const &, string const &, double). The only
     * difference is that this version does not require a file with MC-truth pile-up
     * distributions, and reweighting is performed with the nominal MC distribution.
     */
    WeightPileUp(std::string const &name, std::string const &dataPUFileName, double systError);
    
    /// A short-cut for the above version with a default name "WeightPileUp"
    WeightPileUp(std::string const &dataPUFileName, double systError);
    
    /// Default copy constructor
    WeightPileUp(WeightPileUp const &) = default;
    
    /// Default move constructor
    WeightPileUp(WeightPileUp &&) = default;
    
    /// Assignment operator is deleted
    WeightPileUp &operator=(WeightPileUp const &) = delete;
    
    /// Trivial destructor
    virtual ~WeightPileUp() noexcept;

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
    
    /// Returns weights for the current event
    WeightPileUp::Weights const &GetWeights() const;
    
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
    
    /// Nominal and systematic weights calculated for the current event
    Weights weights;
};

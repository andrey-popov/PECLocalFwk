/**
 * \file WeightPileUp.hpp
 * 
 * The module provides an implementation for pile-up reweighting.
 */

#pragma once

#include <PECFwk/core/WeightPileUpInterface.hpp>

#include <TFile.h>
#include <TH1.h>

#include <string>
#include <memory>


/**
 * \class WeightPileUp
 * \brief An implementation for reweighting on additional pp interactions ("pile-up")
 * 
 * The class implements reweighting over pile-up based on the "true" number of pile-up interactions
 * (i.e. the parameter of Poisson distribution, from which the actual number of pile-up events is
 * sampled). The idea follows the official recipe [1].
 * [1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupMCReweightingUtilities
 * 
 * The input parameters for the algorithm are a file with target (data) distribution over pile-up
 * (normally the file is created by script pileupCalc.py [2]) and an amount of scaling of the number
 * of pile-up interactions that is used to account for the systematical variation (as described in
 * [3]). Several times a random-number engine exploited to admix pile-up to MC has been
 * misconfigured, which made the actual pile-up distribution in MC deviate from the nominal one.
 * The class is capable of handling such cases and for this reason can accept the name of a ROOT
 * file with actual pile-up MC histograms, which are used instead of nominal one.
 * [2] https://twiki.cern.ch/twiki/bin/viewauth/CMS/PileupJSONFileforData
 * [3] https://twiki.cern.ch/twiki/bin/view/CMS/PileupSystematicErrors
 * 
 * The nominal MC pile-up distribution used by default is S10 (adopted in Summer12 campaign).
 */
class WeightPileUp: public WeightPileUpInterface
{
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
    WeightPileUp(std::string const &dataPUFileName, std::string const &mcPUFileName,
     double systError);
    
    /**
     * \brief Constuctor
     * 
     * See documentation for WeightPileUp(string const &, string const &, double). The only
     * difference is that this version does not require a file with MC-truth pile-up
     * distributions, and reweighting is performed with the nominal MC distribution.
     */
    WeightPileUp(std::string const &dataPUFileName, double systError);
    
    /// Copy constructor
    WeightPileUp(WeightPileUp const &src);

public:
    /**
     * \brief Returns a newly-initialized copy of the class instance
     * 
     * Consult documentation of the overriden method in the base class for details.
     */
    WeightPileUpInterface *Clone() const;
    
    /**
     * \brief Notifies this of the dataset that is about to be processed
     * 
     * Consult documentation of the overriden method in the base class for details.
     */
    void SetDataset(Dataset const &dataset);
    
    /**
     * \brief Calculates event weight given the "true" number of pile-up interactions
     * 
     * Consult documentation of the overriden method in the base class for details.
     */
    WeightPileUpInterface::Weights GetWeights(double nTruth) const;

private:
    /// Target pile-up distribution in real data
    std::shared_ptr<TH1> dataPUHist;
    
    /// File with MC-truth pile-up distributions
    std::shared_ptr<TFile> mcPUFile;
    
    /// Distribution used in generation of the current MC dataset
    std::shared_ptr<TH1> mcPUHist;
    
    /// Rescaling of the target distribution to estimate systematical uncertainty
    double const systError;
};

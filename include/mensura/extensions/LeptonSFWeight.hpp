#pragma once

#include <mensura/extensions/EventWeightPlugin.hpp>

#include <mensura/core/PhysicsObjects.hpp>

#include <TH2.h>

#include <functional>
#include <list>
#include <memory>


class LeptonReader;


/**
 * \class LeptonSFWeight
 * \brief A plugin to reweight events to account for lepton scale factors
 * 
 * Only leptons of one selected flavour are considered in the computation. If more than one lepton
 * of that flavour are found, their scale factors are multiplied together.
 * 
 * Scale factors are stored in a ROOT file in the form of one or more two-dimensional histograms.
 * Meaning of x and y parameters of each histogram can be specified with a TObjString written in
 * the file. It must have the same name as the histogram, with a postfix "__params" added. The
 * parameter string must be composed of labels defining x and y parameters, separated by a
 * semicolon, for instance "pt;absEta". Following labels are supported: "pt", "eta", "etaSC",
 * "absEta", "absEtaSC". It no parameter string is found, transverse momentum and (signed)
 * pseudorapidity of the lepton are taken parameters of the histogram.
 * 
 * Currently systematic uncertainty is not evaluated.
 */
class LeptonSFWeight: public EventWeightPlugin
{
private:
    /**
     * \brief An auxiliar wrapper around a 2D histogram containing a scale factor
     * 
     * Meaning of x and y parameters of the histogram is not predefined. Instead, values of this
     * parameters are computed using functions that are set at run time.
     */
    struct HistAdjustableParams
    {
        HistAdjustableParams() = default;
        HistAdjustableParams(HistAdjustableParams const &) = default;
        HistAdjustableParams(HistAdjustableParams &&) = default;
        
        /**
         * \brief Histogram with a scale factor
         * 
         * The histogram is shared among all clones of this plugin.
         */
        std::shared_ptr<TH2 const> hist;
        
        /// Functions to evaluate x and y parameters of the histogram
        std::function<double (Lepton const &)> x, y;
    };
    
public:
    /**
     * \brief Creates a reweighting plugin targeting leptons of given flavour
     * 
     * Name of the ROOT file with scale factors is resolved using FileInPath with respect to
     * directory data/LeptonSF/. When multiple histograms are specified, corresponding scale
     * factors are multiplied together. If some of the histograms are not found, an exception is
     * thrown.
     */
    LeptonSFWeight(std::string const &name, Lepton::Flavour targetFlavour,
      std::string const &srcFileName, std::list<std::string> const &histogramNames);
    
    /// A short-cut for the above version with a default name "LeptonSFWeight"
    LeptonSFWeight(Lepton::Flavour targetFlavour, std::string const &srcFileName,
      std::list<std::string> const &histogramNames);
    
    /// Default copy constructor
    LeptonSFWeight(LeptonSFWeight const &) = default;
    
    /// Default move constructor
    LeptonSFWeight(LeptonSFWeight &&) = default;
    
    /// Assignment operator is deleted
    LeptonSFWeight &operator=(LeptonSFWeight const &) = delete;
    
    /// Trivial destructor
    virtual ~LeptonSFWeight() noexcept;
    
public:
    /**
     * \brief Saves pointers to utilized plugins
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
    
private:
    /// Reads histograms with scale factors from the source file
    void LoadScaleFactors(std::string const &srcFileName,
      std::list<std::string> const &histogramNames);
    
    /**
     * \brief Calculates the event weight from the scale factors
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Name of the plugin that produces leptons
    std::string leptonPluginName;
    
    /// Non-owning pointer to the plugin that produces leptons
    LeptonReader const *leptonPlugin;
    
    /// Flavour of leptons, to which the scale factors should be applied
    Lepton::Flavour targetFlavour;
    
    /// Wrappers around histograms containing scale factors
    std::vector<HistAdjustableParams> sfComponents;
};

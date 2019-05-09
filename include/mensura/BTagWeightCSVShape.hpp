#pragma once

#include <mensura/EventWeightPlugin.hpp>

#include <map>
#include <memory>
#include <string>


class JetMETReader;
class TH3;


/**
 * \class BTagWeightCSVShape
 * \brief A plugin that implements a reweighting to CSV shapes
 * 
 * The reweighting is intended to reproduce full shapes of the CSV b-tagging discriminators. Its
 * idea is described in CMS AN-13-130. In the current implementation systematic uncertainties are
 * ignored.
 * 
 * This plugin exploits a JetReader with a default name "JetMET".
 */
class BTagWeightCSVShape: public EventWeightPlugin
{
public:
    /// Creates a service with the given name
    BTagWeightCSVShape(std::string const &name, std::string const &csvWeightFileName,
      double minPt = 0.);
    
    /// A short-cut for the above version with a default name "BTagWeightCSVShape"
    BTagWeightCSVShape(std::string const &csvWeightFileName, double minPt = 0.);
        
    /// Default copy constructor
    BTagWeightCSVShape(BTagWeightCSVShape const &) = default;
    
    /// Default move constructor
    BTagWeightCSVShape(BTagWeightCSVShape &&) = default;
    
    /// Assignment operator is deleted
    BTagWeightCSVShape operator=(BTagWeightCSVShape const &) = delete;
    
    /// Trivial virtual destructor
    virtual ~BTagWeightCSVShape() noexcept;

public:
    /**
     * \brief Performs initialization for a new dataset
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
    /// Reads histograms with scale factors from the given file
    void LoadScaleFactors(std::string const &csvWeightFileName);
    
    /**
     * \brief Calculates weight of the current event.
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;

private:
    /// Name of the plugin that produces jets
    std::string jetPluginName;
    
    /// Non-owning pointer to the plugin that produces jets
    JetMETReader const *jetPlugin;
    
    /// Selection on jet transverse momentum
    double minPt;
    
    /**
     * \brief Histograms with scale factors for CSV reweighting
     * 
     * The key of the map is the jet flavour. Possible values are 5, 4, 0. The histograms are
     * shared among all clones of this object.
     */
    std::map<unsigned, std::shared_ptr<TH3>> csvScaleFactors;
};

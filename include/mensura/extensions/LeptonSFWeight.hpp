#pragma once

#include <mensura/extensions/EventWeightPlugin.hpp>

#include <mensura/core/PhysicsObjects.hpp>

#include <TH2D.h>

#include <list>
#include <memory>


class LeptonReader;


/**
 * \class LeptonSFWeight
 * \brief A plugin to reweight events to account for lepton scale factors
 * 
 * The scale factors are provided as one or more 2D histograms, which encode dependence on lepton
 * transverse momentum and signed (!) pseudorapidity. If several histograms are provided, scale
 * factors read from each of them are multiplied together. The user specifies which lepton flavour
 * is considered. If an event contains several leptons of that flavour (accessed through a
 * LeptonReader with a default name "Leptons"), the final event weight is a multiplication of scale
 * factors for all of the leptons.
 * 
 * Currently no systematic uncertainty is evaluated.
 */
class LeptonSFWeight: public EventWeightPlugin
{
public:
    /**
     * \brief Constructor
     * 
     * The arguments are: name of the plugin, lepton flavour to be considered, name of the ROOT
     * file containing 2D histograms with scale factors, and names of the histograms. The file name
     * is resolved with the help of FileInPath, with a postfix "LeptonSF/". If one of the histogram
     * is not found, the constructor throws an exception.
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
    
    /**
     * \brief Histograms with different components of scale factors
     * 
     * They are shared among all copies of this.
     */
    std::vector<std::shared_ptr<TH2D const>> sfComponents;
};

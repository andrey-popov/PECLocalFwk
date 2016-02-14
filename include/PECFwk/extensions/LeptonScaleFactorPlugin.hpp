/**
 * \file LeptonScaleFactorPlugin.hpp
 * 
 * The module defines a plugin to account of lepton scale factors.
 */

#pragma once

#include <PECFwk/extensions/EventWeightPlugin.hpp>

#include <PECFwk/core/PECReaderPlugin.hpp>

#include <TH2D.h>

#include <list>
#include <memory>


/**
 * \class LeptonScaleFactorPlugin
 * \brief A plugin to reweight events to account for lepton scale factors
 * 
 * The scale factors are provided as one or more 2D histograms, which encode dependence on lepton's
 * transverse momentum and signed (!) pseudorapidity. If several histograms are provided, scale
 * factors read from each of them are multiplied together. The user specifies which lepton flavour
 * is considered. If an event contains several leptons of that flavour (accessed through
 * PECReader::GetLeptons), the final event weight is a multiplication of scale factors for all of
 * the leptons.
 * 
 * Currently no systematic uncertainty is evaluated.
 */
class LeptonScaleFactorPlugin: public EventWeightPlugin
{
public:
    /**
     * \brief Constructor
     * 
     * The arguments are: name of the plugin, lepton flavour to be considered, name of the ROOT file
     * containing 2D histograms with scale factors, and names of the histograms. The file name is
     * resolved with the help of FileInPath, the priority location is
     * $PEC_FWK_INSTALL/data/LeptonSF/. If one of the histogram is not found, the constructor throws
     * an exception.
     */
    LeptonScaleFactorPlugin(std::string const &name, Lepton::Flavour targetFlavour,
     std::string const &srcFileName, std::list<std::string> histogramNames);
    
    /// Copy constructor
    LeptonScaleFactorPlugin(LeptonScaleFactorPlugin const &src);
    
    /// Trivial destructor
    virtual ~LeptonScaleFactorPlugin() noexcept;
    
public:
    /**
     * \brief Returns a newly-initialised copy of this
     * 
     * Consult documentation of the base class Plugin for details on the purpose of this method.
     */
    virtual Plugin *Clone() const;
    
    /**
     * \brief Performs initialisation when a new dataset is opened
     * 
     * Only (re)sets the pointer to the PECReaderPlugin. Consult documentation of the base class
     * Plugin for details on the purpose of this method.
     */
    virtual void BeginRun(Dataset const &);
    
private:
    /**
     * \brief Calculates the event weight from the scale factors
     * 
     * Only central weight is calculated. The method always returns true. Consult documentation of
     * the base class Plugin for details on the purpost of this method.
     */
    virtual bool ProcessEvent();
    
private:
    /// Pointer to PECReaderPlugin
    PECReaderPlugin const *reader;
    
    /// Flavour of leptons, to which the scale factors should be applied
    Lepton::Flavour targetFlavour;
    
    /**
     * \brief Histograms with different components of scale factors
     * 
     * They are shared among all copies of this.
     */
    std::vector<std::shared_ptr<TH2D const>> sfComponents;
};

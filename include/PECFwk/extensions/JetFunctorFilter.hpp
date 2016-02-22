#pragma once

#include <PECFwk/core/AnalysisPlugin.hpp>

#include <PECFwk/core/PhysicsObjects.hpp>

#include <functional>


class JetMETReader;


/**
 * \class JetFunctorFilter
 * \brief Filters events based on the number of jets that pass a generic selection
 * 
 * This plugin selects an event if it contains a desired number of jets that pass a selection. The
 * selection is specified with the help of std::function and, therefore, can be very general.
 * 
 * The filter relies on the presence of a JetMETReader with a default name "JetMET".
 */
class JetFunctorFilter: public AnalysisPlugin
{
public:
    /**
     * \brief Constructor
     * 
     * Arguments are: name of the plugin, a jet selection, minimal and maximal allowed numbers of
     * jets to pass the selection. If the last argument is omitted, there is no upper limit on the
     * number of jets.
     */
    JetFunctorFilter(std::string const &name, std::function<bool(Jet const &)> const &selector,
      unsigned minNumJets, unsigned maxNumJets = -1) noexcept;
    
    /**
     * \brief Constructor
     * 
     * A short-cut for the version above that uses a default name "JetFunctorFilter".
     */
    JetFunctorFilter(std::function<bool(Jet const &)> const &selector,
      unsigned minNumJets, unsigned maxNumJets = -1) noexcept;
    
    /// Copy constructor
    JetFunctorFilter(JetFunctorFilter const &src) noexcept;
    
public:
    /**
     * \brief Performs initialization for a new dataset
     * 
     * Reimplemented from Plugin.
     */
    virtual void BeginRun(Dataset const &);
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Plugin.
     */
    virtual Plugin *Clone() const noexcept;
    
private:
    /**
     * \brief Checks jets in the current event and performs the selection
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent();
    
private:
    /// Name of the plugin that produces jets
    std::string jetPluginName;
    
    /// Non-owning pointer to a plugin that produces jets
    JetMETReader const *jetPlugin;
    
    /// Generic jet selector
    std::function<bool(Jet const &)> const &selector;
    
    /// Minimal number of jets passing the threshold
    unsigned minNumJets, maxNumJets;
};

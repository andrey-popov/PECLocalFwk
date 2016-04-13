/**
 * \file AnalysisPlugin.hpp
 * 
 * The module defines an abstract base class for analysis plugins.
 */

#pragma once

#include <mensura/core/Plugin.hpp>


/**
 * \class AnalysisPlugin
 * \brief Abstract base class for analysis plugins
 * 
 * Compared to the base class Plugin, here a specific logic to interpret boolean decision issued by
 * ProcessEvent method is implemented. The decision is treated as a filter decision. If it is false,
 * the parent Processor does not evaluate for the current event plugins that depend on the given
 * one.
 * 
 * Consult documentation for the base class. In particular, in a derived class user must implement
 * methods Clone and ProcessEvent.
 */
class AnalysisPlugin: public Plugin
{
public:
    /// Constructor
    AnalysisPlugin(std::string const &name);
    
    /// Default copy constructor
    AnalysisPlugin(AnalysisPlugin const &) = default;
    
    /// Default move constructor
    AnalysisPlugin(AnalysisPlugin &&) = default;
    
    /// Default assignment operator
    AnalysisPlugin &operator=(AnalysisPlugin const &) = default;
    
    /// Trivial destructor
    virtual ~AnalysisPlugin();
    
private:
    /**
     * \brief Reinterprets boolean decision issued by ProcessEvent
     * 
     * The return value of ProcessEvent is interpreted as a filter decision of the plugin.
     */
    virtual EventOutcome ReinterpretDecision(bool decision) const override final;
};

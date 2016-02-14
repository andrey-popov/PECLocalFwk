/**
 * \file ReaderPlugin.hpp
 * 
 * The module defines an abstract base class for reader plugins.
 */

#pragma once

#include <PECFwk/core/Plugin.hpp>


/**
 * \class ReaderPlugin
 * \brief Abstract base class for reader plugins
 * 
 * Compared to the base class Plugin, here a specific logic to interpret boolean decision issued by
 * ProcessEvent method is implemented. The decision is treated as an indication of a succeful
 * reading. If the value is false, it means that there are events left in the input dataset, and
 * therefore the parent Processor must terminate the event loop and proceed to the next dataset.
 * 
 * Consult documentation for the base class. In particular, in a derived class user must implement
 * methods Clone and ProcessEvent.
 */
class ReaderPlugin: public Plugin
{
public:
    /// Constructor
    ReaderPlugin(std::string const &name);
    
    /// Default copy constructor
    ReaderPlugin(ReaderPlugin const &) = default;
    
    /// Default move constructor
    ReaderPlugin(ReaderPlugin &&) = default;
    
    /// Default assignment operator
    ReaderPlugin &operator=(ReaderPlugin const &) = default;
    
    /// Trivial destructor
    virtual ~ReaderPlugin();
    
private:
    /**
     * \brief Reinterprets boolean decision issued by ProcessEvent
     * 
     * The return value of ProcessEvent is interpreted as an indication of a successful reading from
     * the current input dataset.
     */
    virtual EventOutcome ReinterpretDecision(bool decision) const override final;
};

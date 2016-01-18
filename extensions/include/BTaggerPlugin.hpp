/**
 * \file BTaggerPlugin.hpp
 * 
 * Defines an abstract base class for a plugin to perform b-tagging.
 */

#pragma once

#include <Plugin.hpp>

#include <PhysicsObjects.hpp>


/**
 * \class BTaggerPlugin
 * \brief An abstract base class for a plugin to perform b-tagging
 * 
 * Standard b-tagging in the framework relies on properties of a single jet only and does not depend
 * on the remaining part of an event. However, in certain cases this rule does not apply. For
 * example, b-tagged jets might be chosen randomly on per-event basis. Such functionality can be
 * implemented only in a plugin. This base class provides an interface for a b-tagging plugin.
 */
class BTaggerPlugin: public virtual Plugin
{
public:
    /**
     * \brief Constructor
     * 
     * Forwards the given name to the constructor of Plugin.
     */
    BTaggerPlugin(std::string const &name) noexcept;
    
    /// Copy constructor
    BTaggerPlugin(BTaggerPlugin const &src) noexcept;
    
    /// Trivial destructor
    virtual ~BTaggerPlugin() noexcept;
     
public:
    /**
     * \brief A placeholder for a method to be called for each new event
     * 
     * In the default implementation it always returns true, i.e. no event is rejected.
     */
    virtual bool ProcessEvent();
    
    /**
     * \brief Checks if the given jet should be considered as b-tagged
     * 
     * The given reference must point to a jet in the collection returned by PECReader::GetJets(),
     * otherwise the behaviour of the overriden method is undefined.
     */
    virtual bool IsTagged(Jet const &jet) const = 0;
};

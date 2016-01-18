/**
 * \file StdBTaggerPlugin.hpp
 * 
 * Defines a class to wrap the standard b-tagging into a plugin.
 */

#pragma once

#include <BTaggerPlugin.hpp>

#include <BTagger.hpp>

#include <memory>


/**
 * \class StdBTaggerPlugin
 * \brief Wraps the standard b-tagging into a plugin
 * 
 * A concrete implementation of BTaggerPlugin that simply wraps the standard class to perform
 * b-tagging into a plugin. The user is expected to use this class only to complement another
 * non-trivial implementation of BTaggerPlugin. If, instead, there is no specific need in that
 * class and only standard b-tagging is exploited, the user is advised to use the BTagger class.
 */
class StdBTaggerPlugin: public BTaggerPlugin
{
public:
    /**
     * \brief Constructor
     * 
     * Arguments are: desired name of the plugin, b-tagging object, and requested working point.
     */
    StdBTaggerPlugin(std::string const &name, std::shared_ptr<BTagger const> const &bTagger,
     BTagger::WorkingPoint workingPoint) noexcept;
    
    /**
     * \brief Constructor
     * 
     * Arguments are: desired name of the plugin, b-tagging object, and requested working point. The
     * b-tagging object is copied. The first version of constructor should be preferred to this one.
     */
    StdBTaggerPlugin(std::string const &name, BTagger const &bTagger,
     BTagger::WorkingPoint workingPoint) noexcept;
    
    /// Copy constructor
    StdBTaggerPlugin(StdBTaggerPlugin const &src) noexcept;
    
    /// Trivial destructor
    virtual ~StdBTaggerPlugin() noexcept;
     
public:
    /// Clones *this
    virtual Plugin *Clone() const;
    
    /**
     * \brief Checks if the given jet is b-tagged
     * 
     * Only one working point that was provided to the constructor can be used.
     */
    virtual bool IsTagged(Jet const &jet) const;
    
private:
    /// An object to perform b-tagging
    std::shared_ptr<BTagger const> bTagger;
    
    /// Selected working point to perform b-tagging
    BTagger::WorkingPoint workingPoint;
};

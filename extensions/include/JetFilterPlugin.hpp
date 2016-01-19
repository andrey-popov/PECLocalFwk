/**
 * \file JetFilterPlugin.hpp
 *
 * Defines a plugin to filter events based on the number of jets that pass a generic selection.
 */

#pragma once

#include <AnalysisPlugin.hpp>

#include <PECReaderPlugin.hpp>

#include <functional>


/**
 * \class JetFilterPlugin
 * \brief Filters events based on the number of jets that pass a generic selection
 * 
 * The plugin selects an event if it contains a desired number of jets that pass a selection. The
 * selection is specified with the help of std::function and, therefore, can be very general. It
 * checks only jets in the PECReader::GetJets collection.
 */
class JetFilterPlugin: public AnalysisPlugin
{
public:
    /**
     * \brief Constructor
     * 
     * Arguments are: name of a plugin instance, a jet selection, minimal and maximal allowed
     * numbers of jets to pass the selection. If the last argument is omitted, there is no upper
     * limit on the number of jets.
     */
    JetFilterPlugin(std::string const &name, std::function<bool(Jet const &)> const &selection,
     unsigned minNumJets, unsigned maxNumJets = -1) noexcept;
    
    /**
     * \brief Constructor
     * 
     * Same as the above version but constucts the name of the plugin automatically using the given
     * jet counters. If another plugin of this type is instancitated with the same jet counters,
     * there will be a collision of names.
     */
    JetFilterPlugin(std::function<bool(Jet const &)> const &selection,
     unsigned minNumJets, unsigned maxNumJets = -1) noexcept;
    
    /// Copy constructor
    JetFilterPlugin(JetFilterPlugin const &src) noexcept;
    
public:
    /// Constructs a newly-initialised copy
    virtual Plugin *Clone() const noexcept;
    
    /**
     * \brief Notifies *this that a new dataset has been opened
     * 
     * Only updates the pointer to the reader plugin.
     */
    virtual void BeginRun(Dataset const &);
    
private:
    /// Peforms filtering
    virtual bool ProcessEvent();
    
private:
    /// Generic selection on jets
    std::function<bool(Jet const &)> const &selection;
    
    /// Minimal number of jets passing the threshold
    unsigned minNumJets, maxNumJets;
    
    /// Pointer to the reader plugin
    PECReaderPlugin const *reader;
};

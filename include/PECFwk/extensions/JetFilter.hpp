#pragma once

#include <PECFwk/core/AnalysisPlugin.hpp>

#include <string>
#include <vector>


class JetMETReader;


/**
 * \class JetFilter
 * \brief Filter to perform event selection based on jet multiplicity
 * 
 * (Documentation is to be added.)
 * 
 * The filter relies on the presence of a JetMETReader. The default name is "JetMET".
 */
class JetFilter: public AnalysisPlugin
{
private:
    /**
     * \struct SelectionBin
     * \brief An auxiliary struct to describe a rectangle in jet and b tag multiplicity
     */
    struct SelectionBin
    {
        /// Constructor with complete initialization
        SelectionBin(unsigned minJets, unsigned maxJets, unsigned minTags, unsigned maxTags)
          noexcept;
        
        /// Default copy constructor
        SelectionBin(SelectionBin const &) = default;
        
        /// Default move constructor
        SelectionBin(SelectionBin &&) = default;
        
        /// Assignment operator is deleted
        SelectionBin &operator=(SelectionBin const &) = delete;
        
        /// Checks if this bin contains given multiplicities
        bool Contains(unsigned nJets, unsigned nTags) const;
        
        /// Allowed range for jet multiplicity
        unsigned minJets, maxJets;
        
        /// Allowed range for multiplicity of b tags
        unsigned minTags, maxTags;
    };
    
public:
    /// Creates filter plugin with the given name
    JetFilter(std::string const name = "JetFilter") noexcept;
    
    // Default copy constructor
    JetFilter(JetFilter const &) = default;
    
    /// Default move constructor
    JetFilter(JetFilter &&) = default;
    
    /// Assignment operator is disabled
    JetFilter &operator=(JetFilter const &) = delete;
    
    /// Trivial destructor
    virtual ~JetFilter() noexcept;
    
public:
    /// Adds a new selection bin
    void AddSelectionBin(unsigned minJets, unsigned maxJets, unsigned minTags, unsigned maxTags);
    
    /**
     * \brief Adds a new selection bin
     * 
     * The bin consists of a single point in the multiplicity plane.
     */
    void AddSelectionBin(unsigned nJets, unsigned nTags);
    
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
    /**
     * \brief Checks jets in the current event and performs the selection
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Name of the plugin that produces jets
    std::string jetPluginName;
    
    /// Non-owning pointer to a plugin that produces jets
    JetMETReader const *jetPlugin;
    
    /// Selection on jet transverse momentum
    double minPt;
    
    /**
     * \brief Registered selection bins
     * 
     * The order is not important for the algorithm, but prefer vector for a fast iteration.
     */
    std::vector<SelectionBin> bins;
};

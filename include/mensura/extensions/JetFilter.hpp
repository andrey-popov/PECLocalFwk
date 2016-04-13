#pragma once

#include <mensura/core/AnalysisPlugin.hpp>

#include <mensura/core/BTagger.hpp>

#include <string>
#include <vector>


class BTagWPService;
class JetMETReader;


/**
 * \class JetFilter
 * \brief Filter to perform event selection based on jet multiplicity
 * 
 * The filter counts jets above a user-defined pt threshold. Out of the selected jets, it also
 * counts those that are b-tagged according to the provided algorithm and working point. The event
 * is accepted if these two multiplicities fall into one of 2D regions of acceptance (selection
 * bins)provided by the user.
 * 
 * The filter relies on the presence of a JetMETReader and a BTagWPService. The default names are
 * "JetMET" and "BTagWP" respectively.
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
    /**
     * \brief Creates filter with the given name and parameters for jet counting
     * 
     * Only jets with transverse momenta larger than the given threshold are considered. Among
     * them, b-tagged jets are defined by the given b tagger.
     */
    JetFilter(std::string const name, double minPt, BTagger const &bTagger) noexcept;
    
    /// A short-cut for the above version with default name "JetFilter"
    JetFilter(double minPt, BTagger const &bTagger) noexcept;
    
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
    
    /// Non-owning pointer to the plugin that produces jets
    JetMETReader const *jetPlugin;
    
    /// Name of the service that provides b-tagging thresholds
    std::string bTagWPServiceName;
    
    /// Non-owning pointer to the service that provides b-tagging thresholds
    BTagWPService const *bTagWPService;
    
    /// Selection on jet transverse momentum
    double minPt;
    
    /// Definition of a b-tagged jet
    BTagger bTagger;
    
    /**
     * \brief Registered selection bins
     * 
     * The order is not important for the algorithm, but prefer vector for a fast iteration.
     */
    std::vector<SelectionBin> bins;
};

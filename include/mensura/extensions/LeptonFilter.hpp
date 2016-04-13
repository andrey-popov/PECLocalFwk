#pragma once

#include <mensura/core/AnalysisPlugin.hpp>

#include <mensura/core/PhysicsObjects.hpp>

#include <string>
#include <vector>


class LeptonReader;


/**
 * \class LeptonFilter
 * \brief Filter to perform event selection based on basic kinematic properties of leptons
 * 
 * The user defines the selection with a set of selection bins. Each bin specifies pt and |eta|
 * requirements for a given lepton flavour. A bin is added for every lepton whose presence is
 * required by the event selection. An event is accepted if for every bin there is at least one
 * compatible tight lepton, the total number of selected tight leptons equals the number of bins,
 * and there are no additional loose leptons.
 * 
 * For instance, a selection asking for two muons with pt > 20 GeV/c and |eta| < 2.1, one of them
 * having pt > 26 GeV/c, can be constructed with the following commands:
 *   AddSelectionBin(Lepton::Flavour::Muon, 20., 2.1);
 *   AddSelectionBin(Lepton::Flavour::Muon, 26., 2.1);
 * 
 * The filter relies on the presence of a LeptonReader. The default name is "Leptons".
 */
class LeptonFilter: public AnalysisPlugin
{
private:
    /**
     * \struct SelectionBin
     * \brief An auxiliary struct to count leptons of given flavour that meet basic kinematic
     * requirements
     */
    struct SelectionBin
    {
        /// Constructor with complete initialization
        SelectionBin(Lepton::Flavour flavour, double minPt, double maxAbsEta) noexcept;
        
        /// Default copy constructor
        SelectionBin(SelectionBin const &) = default;
        
        /// Default move constructor
        SelectionBin(SelectionBin &&) = default;
        
        /// Assignment operator is deleted
        SelectionBin &operator=(SelectionBin const &) = delete;
        
        /// Resets the counter
        void Clear();
        
        /**
         * \brief Checks if the lepton falls into this bin
         * 
         * The return value indicates if the lepton is compatible with requirements of this bin. If
         * this is the case, the counter is incremented.
         */
        bool Fill(Lepton const &lepton);
        
        /// Flavour of leptons allowed in this bin
        Lepton::Flavour flavour;
        
        /// Minimal requirement on transverse momentum
        double minPt;
        
        /// Maximal allowed absolute value of pseudorapidity
        double maxAbsEta;
        
        /// Number of leptons filled into this bin
        unsigned counts;
    };
    
public:
    /// Creates filter plugin with the given name
    LeptonFilter(std::string const name = "LeptonFilter") noexcept;
    
    /**
     * \brief Creates filter plugin and adds a selection bin
     * 
     * This is a shortcut that calls AddSelectionBin internally.
     */
    LeptonFilter(std::string const &name, Lepton::Flavour flavour, double minPt, double maxAbsEta,
      unsigned duplicates = 1) noexcept;
    
    // Default copy constructor
    LeptonFilter(LeptonFilter const &) = default;
    
    /// Default move constructor
    LeptonFilter(LeptonFilter &&) = default;
    
    /// Assignment operator is disabled
    LeptonFilter &operator=(LeptonFilter const &) = delete;
    
    /// Trivial destructor
    virtual ~LeptonFilter() noexcept;
    
public:
    /**
     * \brief Adds a new selection bin for a lepton with the given flavour
     * 
     * The order in which selection bins are added does not matter. To construct several identical
     * bins (for instance, for a dimuon selection with same kinematical requirements), optional
     * argument 'duplicates' can be utilized.
     */
    void AddSelectionBin(Lepton::Flavour flavour, double minPt, double maxAbsEta,
      unsigned duplicates = 1);
    
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
     * \brief Checks leptons in the current event and performs the selection
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Name of the plugin that produces leptons
    std::string leptonPluginName;
    
    /// Non-owning pointer to a plugin that produces leptons
    LeptonReader const *leptonPlugin;
    
    /**
     * \brief Registered selection bins
     * 
     * The order is not important for the algorithm, but prefer vector for a fast iteration.
     */
    std::vector<SelectionBin> bins;
};

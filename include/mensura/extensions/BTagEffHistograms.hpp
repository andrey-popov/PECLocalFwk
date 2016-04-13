#pragma once

#include <mensura/core/AnalysisPlugin.hpp>

#include <mensura/core/BTagger.hpp>

#include <TH2D.h>

#include <list>
#include <map>
#include <string>
#include <utility>
#include <vector>


class BTagWPService;
class JetMETReader;
class PileUpWeight;
class TFileService;


/**
 * \class BTagEffHistograms
 * \brief Fills histograms that can be used to calculate b-tagging efficiency
 * 
 * This plugin fills histograms with jet distributions in pt and |eta| for each dataset. The
 * histograms are filled for various jet flavours (b, c, other) with all jets in an event and jets
 * passing given working points of a b-tagger. These histograms can then be used to calculate
 * b-tagging efficiencies.
 * 
 * By default, the histograms have very fine binning. It is foreseen that bins will be merged when
 * the efficiencies are calculated.
 * 
 * This plugin only accounts for the event weight due to pile-up.
 * 
 * This plugin depends on a number of services and plugins: BTagWPService (default name "BTagWP"),
 * TFileService ("TFileService"), JetMETReader("JetMET"), PileUpWeight ("PileUpWeight").
 */
class BTagEffHistograms: public AnalysisPlugin
{
private:
    /// An aggregate to combine histograms for jets of the same flavour
    struct HistFlavourGroup
    {
        /**
         * \brief Histogram with all jets (denominator to calculate the b-tagging efficiency)
         * 
         * The object is owned by the output file in TFileService.
         */
        TH2D *denominator;
        
        /**
         * \brief Histograms with jets passing given working point (numerator)
         * 
         * The objects are owned by the output file in TFileService.
         */
        std::map<BTagger::WorkingPoint, TH2D *> numerator;
    };
    
public:
    /// Creates a plugin with the given name and b-tagging configuration
    BTagEffHistograms(std::string const &name, BTagger::Algorithm algo,
      std::list<BTagger::WorkingPoint> const &workingPoints);
    
    /// A short-cut for the above version with a default name "BTagEffHistograms"
    BTagEffHistograms(BTagger::Algorithm algo,
      std::list<BTagger::WorkingPoint> const &workingPoints);
    
    /// Default copy constructor
    BTagEffHistograms(BTagEffHistograms const &) = default;
    
    /// Default move constructor
    BTagEffHistograms(BTagEffHistograms &&) = default;
    
    /// Assignment operator is deleted
    BTagEffHistograms &operator=(BTagEffHistograms const &) = delete;
    
    /// Trivial destructor
    virtual ~BTagEffHistograms() noexcept;
    
public:
    /**
     * \brief Creates histograms for the new dataset
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
    
    /**
     * \brief Clears container with histograms
     * 
     * Reimplemented from Plugin.
     */
    virtual void EndRun() override;
    
    /// Changes binning in absolute value of pseudorapidity
    void SetEtaBinning(std::vector<double> const &etaBinning);
    
    /// Changes binning in transverse momentum
    void SetPtBinning(std::vector<double> const &ptBinning);
    
private:
    /// Performs initialization such as setting default binning for histograms
    void Initialize();
    
    /**
     * \brief Loops over jets in the current event and fills the histograms
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Selected b-tagging algorithm
    BTagger::Algorithm algo;
    
    /// Working points for which histograms need to be filled
    std::list<BTagger::WorkingPoint> workingPoints;
    
    /// Binning in transverse momentum
    std::vector<double> ptBinning;
    
    /// Binning in pseudorapidity
    std::vector<double> etaBinning;
    
    /// Name of TFileService
    std::string fileServiceName;
    
    /// Non-owning pointer to TFileService
    TFileService const *fileService;
    
    /// Name of the plugin that produces jets
    std::string jetPluginName;
    
    /// Non-owning pointer to the plugin that produces jets
    JetMETReader const *jetPlugin;
    
    /// Name of the service that provides b-tagging working points
    std::string bTagWPServiceName;
    
    /// Non-owning pointer to the service that provides b-tagging working points
    BTagWPService const *bTagWPService;
    
    /// Name of the plugin that performs pile-up reweighting
    std::string puWeightPluginName;
    
    /// Non-owning pointer to the plugin that performs pile-up reweighting
    PileUpWeight const *puWeightPlugin;
    
    /**
     * \brief Distributions of all and b-tagged jets with the given flavour
     * 
     * The key of the map is the absolute value of jet flavour. Supported values are 0, 4, 5.
     */
    std::map<unsigned, HistFlavourGroup> histGroups;
};

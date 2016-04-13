#pragma once

#include <mensura/core/AnalysisPlugin.hpp>

#include <mensura/core/BTagger.hpp>
#include <mensura/extensions/BTagSFService.hpp>

#include <memory>
#include <string>


class BTagEffService;
class BTagWPService;
class JetMETReader;


/**
 * \class BTagWeight
 * \brief A plugin that implements a reweighting to account for b-tagging scale factors
 * 
 * Reweighting is performed according to a recipe described in [1]. Tag configuration is not
 * affected, i.e. if a jet is b-tagged, it stays b-tagged after the reweighting is applied. The
 * user can provide a pt threshold, which a jet much satisfy in order to be considered in the
 * procedure.
 * [1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagSFMethods?rev=27#1a_Event_reweighting_using_scale
 * 
 * This plugin exploits a JetReader (default name is "JetMET") and a number of services to access
 * b-tagging working points ("BTagWP"), efficiencies ("BTagEff"), and scale factors ("BTagSF").
 */
class BTagWeight: public AnalysisPlugin
{
public:
    /// Supported systematical variations
    enum class Variation
    {
        Nominal,        ///< Nominal weight
        TagRateUp,      ///< Scale factors for b- and c-jets increased
        TagRateDown,    ///< Scale factors for b- and c-jets decreased
        MistagRateUp,   ///< Scale factors for light-flavour and gluon jets increased
        MistagRateDown  ///< Scale factors for light-flavour and gluon jets decreased
    };
    
public:
    /// Creates a service with the given name
    BTagWeight(std::string const &name, BTagger bTagger, double minPt = 0.);
    
    /// A short-cut for the above version with a default name "BTagWeight"
    BTagWeight(BTagger bTagger, double minPt = 0.);
        
    /// Default copy constructor
    BTagWeight(BTagWeight const &) = default;
    
    /// Default move constructor
    BTagWeight(BTagWeight &&) = default;
    
    /// Assignment operator is deleted
    BTagWeight operator=(BTagWeight const &) = delete;
    
    /// Trivial virtual destructor
    virtual ~BTagWeight() noexcept;

public:
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
    
    /**
     * \brief Calculates event weight
     * 
     * Calculates an event weight needed to account for b-tagging scale factors. The tag
     * configuration is not modified: if a jet is b-tagged, it is considered as b-tagged for
     * both MC and data.
     */
    double CalcWeight(Variation var = Variation::Nominal) const;
    
private:
    /**
     * \brief Does nothing because computation of event weights is delegated to CalcWeight
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
    /**
     * \brief Reinterprets given variation in terms of BTagSFService::Variation enumeration
     * 
     * Provided PDG ID value can be negative. For a mismatched combination, e.g. asking for a
     * TagRateUp variation for a gluon jet, BTagSFService::Variation::Nominal is returned.
     */
    static BTagSFService::Variation TranslateVariation(Variation var, int jetPdgId);

private:
    /// Name of the plugin that produces jets
    std::string jetPluginName;
    
    /// Non-owning pointer to the plugin that produces jets
    JetMETReader const *jetPlugin;
    
    /// Name of the service that provides b-tagging thresholds
    std::string bTagWPServiceName;
    
    /// Non-owning pointer to the service that provides b-tagging thresholds
    BTagWPService const *bTagWPService;
    
    /// Name of the service that provides b-tagging efficiencies
    std::string bTagEffServiceName;
    
    /// Non-owning pointer to the service that provides b-tagging efficiencies
    BTagEffService const *bTagEffService;
    
    /// Name of the service that provides b-tagging scale factors
    std::string bTagSFServiceName;
    
    /// Non-owning pointer to the service that provides b-tagging scale factors
    BTagSFService const *bTagSFService;
    
    /// Definition of a b-tagged jet
    BTagger bTagger;
    
    /// Selection on jet transverse momentum
    double minPt;
};

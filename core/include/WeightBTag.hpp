/**
 * \file WeightBTag.hpp
 * \author Andrey Popov
 * 
 * Defines a class to evaluate event weight due to b-tagging.
 */

#pragma once

#include <WeightBTagInterface.hpp>

#include <BTagger.hpp>
#include <BTagEffInterface.hpp>
#include <BTagSFInterface.hpp>


/**
 * \class WeightBTag
 * \brief Implements a reweighting to account for b-tagging scale factors
 * 
 * Reweighting is performed according to a recipe described in [1]. It is a simplification of a
 * prescription known in single-top group as "Rizzi recipe". In contrast to it and to the previous
 * incarnation of the class (which implemented so-called "extended Rizzi recipe"), a single tag
 * configuration is only considered and not the set of configurations that allow the event to pass
 * the selection. If a jet is tagged, it is considered tagged in all the sums. Therefore, there is
 * no need to specify the event selection to this class.
 * [1] https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods#1a_Event_reweighting_using_scale
 */
class WeightBTag: public WeightBTagInterface
{
public:
    /**
     * \brief Constructor
     */
    WeightBTag(BTagger const *bTagger, BTagEffInterface *efficiencies,
     BTagSFInterface *scaleFactors);

public:
    /**
     */
    virtual WeightBTagInterface *Clone() const;
    
    /**
     */
    virtual void LoadPayload(Dataset const &dataset);
    
    /**
     * \brief Calculates event weight
     * 
     * Calculates an event weight needed to account for b-tagging scale factors. The tags
     * configuration is not modified: if a jet is b-tagged, it is considered as b-tagged for
     * both MC and data.
     */
    virtual double CalcWeight(std::vector<Jet> const &jets, Variation var = Variation::Nominal)
     const;

private:
    /// An object to choose b-tagged jets
    BTagger const *bTagger;
    
    /// An object to access b-tagging efficiencies
    BTagEffInterface *efficiencies;
    
    /// An object to access b-tagging scale factors
    BTagSFInterface *scaleFactors;
};

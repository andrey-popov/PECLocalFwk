/**
 * \file WeightBTag.hpp
 * \author Andrey Popov
 * 
 * The module defines a class to account for b-tagging scale factors.
 */

#pragma once

#include <BTagger.hpp>
#include <BTagDatabase.hpp>
#include <PhysicsObjects.hpp>

#include <vector>


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
class WeightBTag
{
    public:
        /// Supported systematical variations
        enum class Variation
        {
            Central,  ///< Nominal weight
            TagRateUp,  ///< Scale factors for b- and c-jets increased
            TagRateDown,  ///< Scale factors for b- and c-jets decreased
            MistagRateUp,  ///< Scale factors for light-flavour and gluon jets increased
            MistagRateDown  ///< Scale factors for light-flavour and gluon jets decreased
        };
    
    public:
        WeightBTag(BTagger const &bTagger_, BTagDatabase const &bTagDatabase_);
    
    public:
        /// Sets the default variation
        void SetDefaultVariation(Variation var);
        
        /**
         * \brief Calculates event weight
         * 
         * The only difference from the sibling method CalcWeight is that this one uses the default
         * variation.
         */
        double CalcWeight(std::vector<Jet> const &jets) const;
        
        /**
         * \brief Calculates event weight
         * 
         * Calculates an event weight needed to account for b-tagging scale factors. The tags
         * configuration is not modified: if a jet is b-tagged, it is considered as b-tagged for
         * both MC and data.
         */
        double CalcWeight(std::vector<Jet> const &jets, Variation var) const;
    
    private:
        /// Reinterprets a variation in terms of BTagDatabase::SFVar enumeration
        BTagDatabase::SFVar TranslateVariation(Variation var, int jetPDGID) const;
    
    private:
        /// An object to choose b-tagged jets
        BTagger const &bTagger;
        /// An object to access b-tagging efficiencies and scale factors
        BTagDatabase const &bTagDatabase;
        /// Default variation
        Variation defaultVar;
};

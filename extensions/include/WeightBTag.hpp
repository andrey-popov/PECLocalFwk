/**
 * \file WeightBTag.hpp
 * 
 * Defines a class to evaluate event weight due to b-tagging.
 */

#pragma once

#include <WeightBTagInterface.hpp>

#include <BTagger.hpp>
#include <BTagEffInterface.hpp>
#include <BTagSFInterface.hpp>

#include <memory>


/**
 * \class WeightBTag
 * \brief Implements a reweighting to account for b-tagging scale factors
 * 
 * Reweighting is performed according to a recipe described in [1]. Tag configuration is not
 * affected, i.e. if a jet is b-tagged, it stays b-tagged after the reweighting is applied.
 * [1] https://twiki.cern.ch/twiki/bin/view/CMS/BTagSFMethods#1a_Event_reweighting_using_scale
 * 
 * An instance of this class contains unique copies of objects to access b-tagging efficiencies and
 * scale factors. On the other hand, the object to perform b-tagging is shared among all copies of
 * *this.
 * 
 * The class is copyable, movable, non-assignable, and it is not thread-safe.
 */
class WeightBTag: public WeightBTagInterface
{
public:
    /**
     * \brief Constructor
     * 
     * The smart pointer to objects to access b-tagging efficiencies and scale factors are moved
     * from the given locations, thus the latter ones are invalidated.
     * 
     * Provided working point is consistently used to calculate the weight. It might be different
     * from the default working points specified in all provided b-tagging objects.
     */
    WeightBTag(std::shared_ptr<BTagger const> &bTagger, BTagger::WorkingPoint workingPoint,
     std::unique_ptr<BTagEffInterface> &efficiencies,
     std::unique_ptr<BTagSFInterface> &scaleFactors);
    
    /**
     * \brief Constructor
     * 
     * Version of the above constructor with references. The referenced objects are copied.
     */
    WeightBTag(std::shared_ptr<BTagger const> &bTagger, BTagger::WorkingPoint workingPoint,
     BTagEffInterface &efficiencies, BTagSFInterface &scaleFactors);
    
    /**
     * \brief Constructor
     * 
     * It calls the first version of constructor with the working point specified in bTagger.
     */
    WeightBTag(std::shared_ptr<BTagger const> &bTagger,
     std::unique_ptr<BTagEffInterface> &efficiencies,
     std::unique_ptr<BTagSFInterface> &scaleFactors);
    
    /**
     * \brief Constructor
     * 
     * It calls the second version of constructor with the working point specified in bTagger.
     */
    WeightBTag(std::shared_ptr<BTagger const> &bTagger,
     BTagEffInterface &efficiencies, BTagSFInterface &scaleFactors);
    
    /**
     * \brief Copy constructor
     * 
     * It clones objects to access b-tagging efficiencies and scale factors.
     */
    WeightBTag(WeightBTag const &src);
    
    /// Move constructor
    WeightBTag(WeightBTag &&src) noexcept;
    
    /**
     * \brief Assignment operator is deleted
     * 
     * Most straightforward implementation of this operator would call assignment operators for
     * abstract classes BTagEffInterface and BTagSFInterface, which are not virtual and could,
     * therefore, break derived objects. For safety reasons, this operator is deleted.
     */
    WeightBTag operator=(WeightBTag const &) = delete;
    
    /// Trivial virtual destructor
    virtual ~WeightBTag() noexcept;

public:
    /// Wrapper for copy constructor
    virtual WeightBTagInterface *Clone() const;
    
    /**
     * \brief Informs *this that a new dataset has been opened
     * 
     * The method updates b-tagging efficiencies to match the new dataset.
     */
    virtual void LoadPayload(Dataset const &dataset);
    
    /**
     * \brief Calculates event weight
     * 
     * Calculates an event weight needed to account for b-tagging scale factors. The tag
     * configuration is not modified: if a jet is b-tagged, it is considered as b-tagged for
     * both MC and data.
     */
    virtual double CalcWeight(std::vector<Jet> const &jets, Variation var = Variation::Nominal)
     const;

private:
    /// An object to choose b-tagged jets
    std::shared_ptr<BTagger const> bTagger;
    
    /// An object to access b-tagging efficiencies
    std::unique_ptr<BTagEffInterface> efficiencies;
    
    /// An object to access b-tagging scale factors
    std::unique_ptr<BTagSFInterface> scaleFactors;
    
    /// Chosen working point of b-tagging algorithm
    BTagger::WorkingPoint workingPoint;
};

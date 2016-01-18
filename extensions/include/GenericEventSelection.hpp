/**
 * \file GenericEventSelection.hpp
 * 
 * The module defines a class to implement a generic event selecion.
 */

#pragma once

#include <EventSelectionInterface.hpp>
#include <BTagger.hpp>

#include <list>
#include <array>
#include <map>
#include <memory>


/**
 * \class GenericEventSelection
 * \brief Allows to implement an event selection in a flexible way
 * 
 * Along with the constructor, the user must employ methods AddLeptonThreshold and AddJetTagBin
 * in order to specify the event selection.
 * 
 * Each instance of class PECReader must exploit its own copy of class GenericEventSelection, which
 * can be achieved with the help of method Clone.
 * 
 * Consult documentation for the base class for details of the interface.
 * 
 * The class is copyable.
 */
class GenericEventSelection: public EventSelectionInterface
{
private:
    /**
     * \struct JetTagBin
     * \brief Defines a single bin for jet selection
     */
    struct JetTagBin
    {
        /// Default constructor
        JetTagBin() = default;
        
        /// Constructor with parameters
        JetTagBin(unsigned nJets_, unsigned nTags_);
        
        unsigned nJets;  ///< Number of jets
        unsigned nTags;  ///< Number of b-tagged jets
    };

public:
    /**
     * \brief Constructor
     * 
     * Parameters are jet pt threshold and b-tagging object.
     */
    GenericEventSelection(double jetPtThreshold, std::shared_ptr<BTagger const> const &bTagger);
    
    /**
     * \brief Constructor
     * 
     * Takes the same effect as the above version, but the given b-tagging object is copied.
     */
    GenericEventSelection(double jetPtThreshold, BTagger const &bTagger);
    
    /// Constructor
    GenericEventSelection(double jetPtThreshold, std::shared_ptr<BTagger const> &&bTagger);
    
    /// Default copy constructor
    GenericEventSelection(GenericEventSelection const &) = default;
    
    /// Assignment operator is deleted
    GenericEventSelection &operator=(GenericEventSelection const &) = delete;

public:
    /**
     * \brief Performs the event selection on leptons
     * 
     * Checks the number and the transverse momenta of the tight leptons, vetoes additional
     * loose leptons. For example, if the user has requested two muons with thresholds 25 and
     * 15 GeV/c with the help of method AddLeptonThreshold, then an event will be checked to
     * contain exactly two tight muons with pt > 15 GeV/c and, in addition, exactly one of them
     * will be required to have pt > 25 GeV/c.
     * 
     * \note See also the documentation in the base base class.
     */
    virtual bool PassLeptonStep(std::vector<Lepton> const &tightLeptons,
     std::vector<Lepton> const &looseLeptons) const;
    
    /**
     * \brief Performs the event selection on jets
     * 
     * An event is checked against the allowed jet-tag bins. The input collection must be
     * filtered with the help of IsAnalysisJet method. To find the number of b-tagged jets the
     * specified b-tagging object is used.
     */
    virtual bool PassJetStep(std::vector<Jet> const &jets) const;
    
    /**
     * \brief Checks if a jet is to be used in high-level analysis
     * 
     * See also documentation of the overridden method in the base class.
     */
    virtual bool IsAnalysisJet(Jet const &jet) const;
    
    /**
     * \brief Adds additional lepton to the selection
     * 
     * The method increases the number of required leptons of the given flavour by one and
     * sets the pt threshold for the new lepton.
     * 
     * \note The method is implemented in such a way that the underlying lists of thresholds
     * are ordered at every moment.
     */
    void AddLeptonThreshold(Lepton::Flavour flavour, double ptThreshold);
    
    /// Extends the selection on jets with a given jet-tag bin
    void AddJetTagBin(unsigned nJets, unsigned nTags);
    
    /**
     * \brief Creates a newly-initialized copy of this
     * 
     * Consult documentation for the base class for details.
     */
    EventSelectionInterface *Clone() const;

private:
    /// Map from lepton flavours to integers starting from zero
    std::map<Lepton::Flavour, unsigned> flavourMap;
    
    /// Iterators of the current positions in the lists of lepton pt thresholds
    mutable std::array<std::list<double>::const_iterator, 3> leptonThresholdIts;
    
    /// The three lists of lepton pt thresholds (one for each lepton flavour)
    std::array<std::list<double>, 3> leptonPtThresholds;
    
    double jetPtThreshold;  ///< Minimum pt for analysis-level jets
    std::shared_ptr<BTagger const> bTagger;  ///< The b-tagging object
    std::list<JetTagBin> jetBins;  ///< List of allowed jet-tag bins
};

/**
 * \file EventSelectionInterface.hpp
 * \author Andrey Popov
 * 
 * The module defines an interface to specify an event selection.
 */

#pragma once

#include <PhysicsObjects.hpp>

#include <vector>


/**
 * \class EventSelectionInterface
 * \brief An abstract class to define an offline event selection
 * 
 * This abstract class defines an interface to specify an event selection. It is not allowed to
 * access quality of objects, which is to be addressed in class PECReader. Instead, it bases the
 * event selection on simple kinematic information stored in PhysicsObjects classes.
 * 
 * The event selection is two-step: leptons are addressed first, then jets. Currently there is no
 * way to define QCD control region, see some thoughts here [1].
 * [1] https://svnweb.cern.ch/trac/singletop/wiki/PECReaderNotes
 * 
 * When several instances of class PECReader are run in parallel, each one is expected to operate
 * its own copy of the class in order to guarantee thread safety and allow derived classes to store
 * cached information if needed. The copy is created by method Clone, which must be implemented by
 * the user.
 * 
 * The class is copyable.
 */
class EventSelectionInterface
{
    public:
        /// Default constructor with no arguments
        EventSelectionInterface() = default;
        
        /// Default copy constructor
        EventSelectionInterface(EventSelectionInterface const &) = default;
        
        /// Default virtual destructor
        virtual ~EventSelectionInterface() = default;
    
    public:
        /**
         * \brief The lepton step of the event selection
         * 
         * Performs the lepton step of the event selection. Although the meaning of the two input
         * collections is not restricted by the interface, the tight leptons must be a subset of the
         * loose ones. Both collections must be ordered in pt in decreasing order. Most typically,
         * when this method is redefined in a derieved class, it will simply count the tight and the
         * loose leptons. The method should return true if the event passes the selection.
         */
        virtual bool PassLeptonStep(std::vector<Lepton> const &tightLeptons,
         std::vector<Lepton> const &looseLeptons) const = 0;
        
        /**
         * \brief The jet step of the event selection
         * 
         * Performs the jet step of the event selection. Normally, the provided collection should
         * contain the jets selected by IsAnalysisJet method only, although the user is allowed to
         * deviate from this recommendation. The collection must be sorted in pt in the decreasing
         * order. The method returns true if the event passes the selection.
         */
        virtual bool PassJetStep(std::vector<Jet> const &jets) const = 0;
        
        /**
         * \brief Selects the analysis-level jets
         * 
         * An analysis might consider moderately soft jets for some observables, despite these jets
         * do not contribute to jet multiplicity and the most of jet-related observables. This
         * method is intended to filter them out. For an analysis-level jet it should return true.
         * In this class the method is implemented to mark all the jets as analysis-level.
         */
        virtual bool IsAnalysisJet(Jet const &jet) const;
        
        /**
         * \brief Creates a newly-configured copy of the instance
         * 
         * User must implement this method in a derived class to create an initialized copy of the
         * instance of the class. Logic of processing algorithm and its configuration only must be
         * duplicated, but the current state might be ignored. The method is expected to be only
         * called before the first event is processed (i.e. before any call to PassLeptonStep,
         * PassJetStep, or IsAnalysisJet).
         */
        virtual EventSelectionInterface *Clone() const = 0;
};
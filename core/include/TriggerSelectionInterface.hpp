/**
 * \file TriggerSelectionInterface.hpp
 * \author Andrey Popov
 * 
 * The module provides an interface to describe a trigger selection.
 */

#pragma once

#include <EventID.hpp>
#include <PECReaderForward.hpp>


// Forward declarations
class TTree;


/**
 * \class TriggerSelectionInterface
 * \brief An abstract class to define interface for the trigger selection
 * 
 * The trigger selection is splitted into two steps represented by virtual methods PassTrigger and
 * GetWeight. At the first step the decision is taken based on the event ID and information stored
 * in the trigger tree. Normally, it would simply check if appropriate triggers accepted the event.
 * At the second step trigger scale factors are incorporated, and an additional selection using the
 * whole information from the event can be evaluated.
 * 
 * When a new dataset is opened, an instanse of this class must be notified with the help of method
 * UpdateTree. It is also given a pointer to the new trigger tree. The tree is not owned by the
 * object (normally, it is owned by an instance of the PECReader class), but all operations on it
 * must be carried in a class derived from this one. In particular, it includes setting of the tree
 * branches' addresses and reading the tree.
 * 
 * If several instances of class PECReader are used simultaneously, each must operate its own copy
 * of a class derived from TriggerSelectionInterface because it contains some information specific
 * for the current file under processing. In order to enable this feature, a derived class must
 * provide an implementation for method Clone.
 */
class TriggerSelectionInterface
{
    public:
        /// Default constructor
        TriggerSelectionInterface() = default;
        
        /// Default copy constructor
        TriggerSelectionInterface(TriggerSelectionInterface const &) = default;
        
        /// Virtual destructor
        virtual ~TriggerSelectionInterface() = default;
        
        /// Default assignment operator
        TriggerSelectionInterface &operator=(TriggerSelectionInterface const &) = default;
    
    public:
        /**
         * \brief Notifies the object that the new trigger tree was read
         * 
         * The method should be called each time a new file is opened by the PECReader and the
         * trigger tree is updated. An instanse of the TriggerSelectionInterface class may modify
         * the tree provided in a call of this method but does not own it (i.e. it must not delete
         * the tree). Apart from the pointer to the new tree, the method must be given a flag that
         * specifies if the new dataset is real data or simulation.
         * 
         * In the default implementation the method has an empty body.
         */
        virtual void UpdateTree(TTree *triggerTree, bool isData) const;
        
        /**
         * \brief Reads the next event from the trigger tree
         * 
         * Must return true if an event was read successfully, false otherwise.
         */
        virtual bool ReadNextEvent() const = 0;
        
        /**
         * \brief Performs the first step of trigger selection on the current event
         * 
         * The decision relies purely on the event ID and the information stored in the trigger
         * tree. Normally, it is used to determine whether the rest of the event content should be
         * read from the file. This method should be called before PECReader::BuildAndSelectEvent.
         */
        virtual bool PassTrigger(EventID const &eventID) const = 0;
        
        /**
         * \brief Performs the second step of the trigger selection and calculates weight
         * 
         * Performs the additional event selection demanded by the TriggerRange object(s) and
         * calculates the event weight. Must be called after PECReader::BuildAndSelectEvent. The
         * systematical variations (due to uncertainties of the trigger scale factors) are not
         * considered at the moment.
         */
        virtual double GetWeight(PECReader const &reader) const = 0;
        
        /**
         * \brief Returns a copy of an instance of the class
         * 
         * The method must return a properly initialized object, but it might ignore data members
         * that are specific to the current event or source file, i.e. it must duplicate the logic
         * and parameters of the processing algorithm, but not members referring to current state.
         * The method is only expected to be called before the first event is processed and the
         * first file is opened.
         */
        virtual TriggerSelectionInterface *Clone() const = 0;
};

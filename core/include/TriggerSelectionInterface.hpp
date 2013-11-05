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
 * At the second step trigger scale factors are incorporated, and an additional selection that
 * exploits information on the whole event can be evaluated.
 * 
 * When a new dataset is opened, an instanse of this class must be notified with the help of method
 * UpdateTree. It is also given a pointer to the new trigger tree. The tree is not owned by the
 * object (normally, it is owned by an instance of the PECReader class), but all operations on it
 * must be carried in a class derived from this TriggerSelectionInterface. In particular, it implies
 * setting of setting addresses for the tree's branches and reading the tree.
 * 
 * Although a trigger selection is expected to be described by a set of TriggerRange objects, which
 * sets a tight connection between the two classes, it is not specified as a part of the interface
 * to provide the user with more flexibility.
 * 
 * If several instances of class PECReader are used simultaneously, each must operate on its own
 * copy of a class derived from TriggerSelectionInterface because it contains information specific
 * for the current file under processing. In order to allow such behaviour, a derived class must
 * provide an implementation for method Clone.
 */
class TriggerSelectionInterface
{
    public:
        /// Constructor with no arguments
        TriggerSelectionInterface();
        
        /// Copy constructor is deleted
        TriggerSelectionInterface(TriggerSelectionInterface const &) = delete;
        
        /// Move constructor
        TriggerSelectionInterface(TriggerSelectionInterface &&src);
        
        /// Virtual destructor
        virtual ~TriggerSelectionInterface() = default;
        
        /// Move assignment operator
        TriggerSelectionInterface &operator=(TriggerSelectionInterface &&rhs);
        
        /// Assignment overator is deleted
        TriggerSelectionInterface &operator=(TriggerSelectionInterface const &) = delete;
    
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
         * Normally, implementation of this method in a derived class is expected to update the
         * pointer to the tree, reset the tree counters, and set addresses of buffers to read the
         * tree.
         */
        virtual void UpdateTree(TTree *triggerTree, bool isData) = 0;
        
        /**
         * \brief Reads the next event from the trigger tree
         * 
         * In case of real data the trigger selection can depend on the data-taking period and hence
         * on the event ID. The ID of the event which is about to be read is known by an instance of
         * the PECReader class and must be provided for this method in order to allow it to read
         * only necessary part of the trigger tree. The method must return true if an event was
         * read successfully and false if there are no more events in the tree.
         */
        virtual bool ReadNextEvent(EventID const &eventID) = 0;
        
        /**
         * \brief Performs the first step of trigger selection on the current event
         * 
         * The decision relies purely on the event ID and the information stored in the trigger
         * tree. Normally, it is used to determine whether the rest of the event content should be
         * read from the file. This method should be called before PECReader::BuildAndSelectEvent.
         */
        virtual bool PassTrigger() const = 0;
        
        /**
         * \brief Performs the second step of the trigger selection and calculates event weight
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
         * The method must return a properly initialised object, but it might ignore data members
         * that are specific to the current event or source file, i.e. it must duplicate the logic
         * and parameters of the processing algorithm, but not members referring to current state.
         * The method is only expected to be called before the first event is processed and the
         * first file is opened.
         */
        virtual TriggerSelectionInterface *Clone() const = 0;
    
    protected:
        /**
         * \brief Pointer to the tree with information on triggers
         * 
         * The tree is read by this but not owned. Therefore, it must not be deleted by this.
         */
        TTree *triggerTree;
        
        /// Total number of entries in the tree
        unsigned long nEntriesTree;
        
        /// Index of the tree entry to be read next
        unsigned long nextEntryTree;
};

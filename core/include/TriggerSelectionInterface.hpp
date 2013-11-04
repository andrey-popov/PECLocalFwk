/**
 * \file TriggerSelectionInterface.hpp
 * \author Andrey Popov
 * 
 * The module provides an interface to describe a trigger selection.
 */

#pragma once

#include <EventID.hpp>
#include <PECReaderForward.hpp>

#include <TClonesArray.h>


/**
 * \class TriggerSelectionInterface
 * \brief An abstract class to define interface for the trigger selection
 * 
 * The trigger selection is splitted into two steps represented by virtual methods PassTrigger and
 * GetWeight. When a new file is started to be processed, method NewFile must be called. The
 * interface tries to incorporate both the data and MC trigger selections, although it implies
 * some unnecessary complications. The concept might be changed in future.
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
         * \brief Should be called each time a new file is started
         * 
         * Default impementation is trivial
         */
        virtual void NewFile(bool isData) const;
        
        /**
         * \brief Performs the first step of trigger selection
         * 
         * The decision relies purely on the information stored in the event-ID and trigger trees.
         * Normally, it is used to determine whether the rest of the event contect should be read
         * from the file. This method should be called before PECReader::BuildAndSelectEvent.
         */
        virtual bool PassTrigger(EventID const &eventID, TClonesArray const *names,
         Bool_t const *fired) const = 0;
        
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
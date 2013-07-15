/**
 * \file TriggerSelection.hpp
 * \author Andrey Popov
 * 
 * The module implements classes to perform a trigger selection in case of real data and simulation.
 */

#pragma once

#include <TriggerSelectionInterface.hpp>

#include <memory>


/**
 * \struct TriggerRangeWrapper
 * \brief An auxiliary struct that combines an instance of class TriggerRange with technical
 * cache information exploited by classes TriggerSelectionData and TriggerSelectionMC
 */
struct TriggerRangeWrapper
{
    /// Constructor
    TriggerRangeWrapper(TriggerRange const &data_);
    
    /// Default copy constructor
    TriggerRangeWrapper(TriggerRangeWrapper const &) = default;
    
    /// Default assignment operator
    TriggerRangeWrapper &operator=(TriggerRangeWrapper const &) = default;
    
    TriggerRange const &data;  ///< Reference to the enclosed TriggerRange object
    unsigned index;  ///< Index of the trigger in the arrays provided by PECReader
    bool passTrigger;  ///< Specifies whether the current event passes the trigger
};



/**
 * \class TriggerSelectionData
 * \brief Implements a trigger selection for real data
 * 
 * The class implements a generic trigger selection for real data. ID for a given event determines
 * the trigger to check the event against. Additional event selection defined in TriggerRange is
 * also evaluated.
 */
class TriggerSelectionData
{
    public:
        /**
         * \brief Constructor
         * 
         * The argument is a list of objects of class TriggerRangeWrapper that belongs to a parent
         * instance of class TriggerSelection. The selection is completely defined by embedded
         * instances of class TriggerRange.
         */
        TriggerSelectionData(std::list<TriggerRangeWrapper> &ranges);
        
        /// Default copy constructor
        TriggerSelectionData(TriggerSelectionData const &) = default;
        
        /// Default assignment operator
        TriggerSelectionData &operator=(TriggerSelectionData const &) = default;
    
    public:
        /// Checks if the event passes selection on trigger bits
        bool PassTrigger(EventID const &eventID, TClonesArray const *names, Bool_t const *fired)
         const;
        
        /**
         * \brief Checks if an event passes an additional selection
         * 
         * Checks if an event passes an additional selection (TriggerRange::PassEventSelection)
         * associated with the chosen trigger. Returns either 0. or 1.
         */
        double GetWeight(PECReader const &reader) const;
    
    private:
        /**
         * \brief Reference to a list of wrapped instances of class TriggerRange
         * 
         * The list belongs to parent class TriggerSelection.
         */
        std::list<TriggerRangeWrapper> &ranges;
        
        /// Iterator pointing to the current TriggerRange object
        mutable std::list<TriggerRangeWrapper>::iterator currentRange;
};


/**
 * \class TriggerSelectionMC
 * \brief Implements a trigger selection for simulation
 * 
 * The class implements a generic trigger selection for simulated event. An event is checked against
 * all the requested triggers. At first approximation, weight of the event is calculated as a sum
 * of integrated luminosities for all triggers that are fired by the event. In addition to this
 * simplified view, trigger scale factors are included. Each TriggerRange might define an additional
 * event selection with reconstructed objects, which is also evaluated.
 * 
 * Implementation profits from the fact that simulation is generated with one trigger menu. For
 * this reason, the trigger indices in the array of decision provided to method PassTrigger are
 * updated when a new file is opened only.
 */
class TriggerSelectionMC
{
    public:
        /**
         * \brief Constructor
         * 
         * The argument is a list of objects of class TriggerRangeWrapper that belongs to a parent
         * instance of class TriggerSelection. The selection is completely defined by embedded
         * instances of class TriggerRange.
         */
        TriggerSelectionMC(std::list<TriggerRangeWrapper> &ranges);
        
        /// Default copy constructor
        TriggerSelectionMC(TriggerSelectionMC const &) = default;
        
        /// Default assignment operator
        TriggerSelectionMC &operator=(TriggerSelectionMC const &) = default;
    
    public:
        /// Reads an array of trigger names and recalculates indices of selected triggers
        void UpdateTriggerIndices(TClonesArray const *names);
        
        /// Checks if an event passes selection on trigger bits
        bool PassTrigger(Bool_t const *fired) const;
        
        /**
         * \brief Performs the additional event selection and calculates event weigth
         * 
         * All the TriggerRange objects are considered. The weight is calculated as the sum of
         * integrated luminosities multiplied by the scale factors; the sum is taken over those
         * ranges for which the event passes the additional selection and fires the corresponding
         * MC trigger. No systematical variation is implemented.
         */
        double GetWeight(PECReader const &reader) const;
    
    protected:
        /**
         * \brief Calculates the trigger scale factor
         * 
         * This is a placeholder which always returns 1. The user can override this method. The
         * functionality of the class should also be extended to allow to specify arbitrary
         * callable expression, similar to how it is done in TriggerRange class. Note the
         * systematical variations are not implemented and are likely to require changes in the
         * trigger interface.
         */
        virtual double ScaleFactor(TriggerRange const &range, PECReader const &reader) const;
    
    private:
        /**
         * \brief Reference to a list of wrapped instances of class TriggerRange
         * 
         * The list belongs to parent class TriggerSelection.
         */
        std::list<TriggerRangeWrapper> &ranges;
};


/**
 * \class TriggerSelection
 * \brief A class to perform trigger selection
 * 
 * The class provides an implementation for TriggerSelectionInterface. The actual event selection
 * is delegated to specialised classes TriggerSelectionData and TriggerSelectionMC. For details
 * consult documentation for these classes as well as the base class.
 */
class TriggerSelection: public TriggerSelectionInterface
{
    public:
        /// Constructor
        TriggerSelection(std::list<TriggerRange> const &ranges);
        
        /// Constructor
        template<typename InputIt>
        TriggerSelection(InputIt const &begin, InputIt const &end);
        
        /// Move constructor
        TriggerSelection(TriggerSelection &&src);
        
        /// Copy constructor
        TriggerSelection(TriggerSelection const &src);
    
    public:
        /**
         * \brief With the help of this class the caller notifies the class a new source file has
         * been opened
         * 
         * Consult documentation in the base class for a description of the interface.
         */
        void NewFile(bool isData) const;
        
        /**
         * \brief Performs event selection over trigger bits
         * 
         * Consult documentation in the base class for a description of the interface.
         */
        bool PassTrigger(EventID const &eventID, TClonesArray const *names,
         Bool_t const *fired) const;
        
        /**
         * \brief Calculates event weight
         * 
         * Performs additional event selection at recontruction level (calls
         * TriggerRange::PassEventSelection). In case of simulation, on top of it the integrated
         * luminosity collected with the given trigger and trigger scale factors are included. See
         * documentation for methods of the same name in classes TriggerSelectionData and
         * TriggerSelectionMC and the overriden method in the base class.
         */
        double GetWeight(PECReader const &reader) const;
        
        /**
         * \brief Creates a newly-initilized copy
         * 
         * Consult documentation of the base class
         */
        TriggerSelectionInterface *Clone() const;
    
    private:
        /// A set of objects of class TriggerRange complemented with auxiliary information
        std::list<TriggerRangeWrapper> ranges;
        
        /// Object to perform event selection in data
        std::unique_ptr<TriggerSelectionData> dataSelection;
        
        /// Object to perform event selection in simulation
        std::unique_ptr<TriggerSelectionMC> mcSelection;
        
        /// Specifies whether the current file is real data or simulation
        mutable bool isData;
        
        /// Specifies whether a new file has been opened
        mutable bool isReset;
};


template<typename InputIt>
TriggerSelection::TriggerSelection(InputIt const &begin, InputIt const &end):
    isReset(false)
{
    for (InputIt it = begin; it != end; ++it)
            ranges.emplace_back(*it);
    
    dataSelection.reset(new TriggerSelectionData(ranges));
    mcSelection.reset(new TriggerSelectionMC(ranges));
}
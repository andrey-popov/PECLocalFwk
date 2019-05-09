#pragma once

#include <mensura/AnalysisPlugin.hpp>

#include <mensura/TriggerRange.hpp>

#include <Rtypes.h>

#include <deque>
#include <map>
#include <stdexcept>
#include <utility>
#include <vector>


class PECInputData;
class TTree;


/**
 * \class PECTriggerFilter
 * \brief An abstract base class to perform trigger selection
 */
class PECTriggerFilter: public AnalysisPlugin
{
public:
    /// Creates plugin with the given name
    PECTriggerFilter(std::string const name = "TriggerFilter");
    
    /// Assignment operator is deleted
    PECTriggerFilter &operator=(PECTriggerFilter const &) = delete;
    
public:
    /**
     * \brief Requests reading of the tree with trigger information
     * 
     * Reimplemented from Plugin.
     */
    virtual void BeginRun(Dataset const &) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Overrides method from Plugin.
     */
    virtual Plugin *Clone() const override = 0;

    /**
     * \brief Calculates weight of the current event
     * 
     * In the default implementation returns 1.
     */
    virtual double GetWeight() const;
    
protected:
    /// Name of the plugin that reads PEC files
    std::string inputDataPluginName;
    
    /// Non-owning pointer to a plugin that reads PEC files
    PECInputData const *inputDataPlugin;
    
    /// Name of the tree containing trigger information
    std::string triggerTreeName;
    
    /// Non-owning pointer to the tree with trigger information
    TTree *triggerTree;
};



/**
 * \class PECTriggerFilterData
 * \brief Implements a trigger selection for data
 * 
 * The selection is described by a collection of TriggerRange objects. They must provide a valid
 * set of data triggers for each event in the dataset that will be processed. If one of the
 * triggers is not found in the trigger tree, an exception is thrown.
 * 
 * Only necessary branches of the trigger tree are read. The GetWeight method always returns 1.
 */
class PECTriggerFilterData: public PECTriggerFilter
{
public:
    /**
     * \brief Constructs a filter with the given name and trigger selection
     * 
     * The trigger selection is described by an STL collection of TriggerRange objects. Internally,
     * only pointers to the TriggerRange objects are stored; therefore, the objects must be
     * available during the lifetime of PECTriggerFilter. They are not owned by this object. The
     * trigger ranges are expected not to overlap. Otherwise the behaviour of trigger selection is
     * undefined.
     */
    template<typename C>
    PECTriggerFilterData(std::string const &name, C const &ranges);
    
    /** 
     * \brief Construct a filter with the given trigger selection
     * 
     * A short-cut for the above version with a default name "TriggerFilter".
     */
    template<typename C>
    PECTriggerFilterData(C const &ranges);
    
public:
    /**
     * \brief Requests reading of the tree with trigger information
     * 
     * Throws an exception if the given dataset is simulated.
     * 
     * Reimplemented from PECTriggerFilter.
     */
    virtual void BeginRun(Dataset const &dataset) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Reimplemented from PECTriggerFilter.
     */
    virtual Plugin *Clone() const override;
    
private:
    /// Stores pointers to provided TriggerRange objects in the internal collection
    template<typename C>
    void ConstructRanges(C const &ranges);
    
    /**
     * \brief Checks if the data trigger in the current trigger range accepts the event
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Non-owning pointers to TriggerRange objects that define the trigger selection
    std::vector<TriggerRange const *> ranges;
    
    /**
     * \brief Non-owning pointer to the current TriggerRange object
     * 
     * If the current event does not match any TriggerRange object or no attempt to find the
     * corresponding range has been made yet, the poiter is null.
     */
    TriggerRange const *currentRange;
    
    /**
     * \brief Buffers into which decision of triggers in the current range are read
     * 
     * Use deque instead of a vector to avoid the vector<bool> specialization.
     */
    std::deque<Bool_t> buffers;
};


template<typename C>
PECTriggerFilterData::PECTriggerFilterData(std::string const &name, C const &ranges_):
    PECTriggerFilter(name),
    currentRange(nullptr)
{
    ConstructRanges(ranges_);
}


template<typename C>
PECTriggerFilterData::PECTriggerFilterData(C const &ranges_):
    PECTriggerFilter("TriggerFilter"),
    currentRange(nullptr)
{
    ConstructRanges(ranges_);
}


template<typename C>
void PECTriggerFilterData::ConstructRanges(C const &ranges_)
{
    // A sanity check
    if (ranges_.size() == 0)
        throw std::logic_error("PECTriggerFilterData::ConstructRanges: Provided collection of "
          "TriggerRange objects is empty.");
    
    
    // Save pointers to the TriggerRange objects
    ranges.reserve(ranges_.size());
    
    for (typename C::const_iterator it = ranges_.cbegin(); it != ranges_.cend(); ++it)
        ranges.push_back(&*it);
}



/**
 * \class PECTriggerFilterMC
 * \brief Implements trigger selection in simulation
 * 
 * The selection is described by a collection of TriggerRange objects. All provided MC triggers
 * must be present, although a special value "1" can be given in place of a trigger name to
 * disable filtering in a certain TriggerRange.
 * 
 * An event passes the trigger selection if it is accepted by at least one of the MC triggers. It
 * is assigned a weight calculated as a sum of integrated luminosities of all data-taking periods
 * whose triggers (at least one if several are defined) accept the event.
 */
class PECTriggerFilterMC: public PECTriggerFilter
{
private:
    /**
     * \brief An auxiliary structure that combines a TriggerRange object with buffers into which
     * decisions of the corresponding triggers are read
     */
    struct TriggerWithBuffer
    {
        /// Constructor with a trivial initialization
        TriggerWithBuffer(TriggerRange const *trigger) noexcept;
        
        /// Non-owning pointer to a TriggerRange object
        TriggerRange const *trigger;
        
        /**
         * \brief Non-owning pointers to buffers into which decision of MC triggers from the
         * TriggerRange object are read
         */
        std::vector<Bool_t *> buffers;
    };
    
public:
    /**
     * \brief Constructs a filter with the given name and trigger selection
     * 
     * The trigger selection is described by an STL collection of TriggerRange objects. Internally,
     * only pointers to the TriggerRange objects are stored; therefore, the objects must be
     * available during the lifetime of PECTriggerFilter. They are not owned by the object.
     */
    template<typename C>
    PECTriggerFilterMC(std::string const &name, C const &ranges);
    
    /** 
     * \brief Construct a filter with the given trigger selection
     * 
     * A short-cut for the above version with a default name "TriggerFilter".
     */
    template<typename C>
    PECTriggerFilterMC(C const &ranges);
    
private:
    /**
     * \brief Default copy constructor
     * 
     * A copy can only be constructed before the first call to BeginRun. For the sake of safety,
     * this constructor is made private.
     */
    PECTriggerFilterMC(PECTriggerFilterMC const &) = default;
    
public:
    /**
     * \brief Requests reading of the tree with trigger information and sets up its branches
     * 
     * Sets up the tree to read decisions of MC triggers in all registered TriggerRange objects.
     * Throws an exception if the given dataset is data.
     * 
     * Reimplemented from PECTriggerFilter.
     */
    virtual void BeginRun(Dataset const &dataset) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Reimplemented from PECTriggerFilter.
     */
    virtual Plugin *Clone() const override;
    
    /**
     * \brief Calculates weight of the current event taking into account integrated luminosity
     * 
     * All the TriggerRange objects are considered. The weight is calculated as a sum of
     * integrated luminosities of those ranges for which the event is accepted by the corresponding
     * MC triggers.
     */
    virtual double GetWeight() const override;
    
private:
    /// Stores pointers to provided TriggerRange objects in the internal collection
    template<typename C>
    void ConstructRanges(C const &ranges);
    
    /**
     * \brief Checks decisions of all MC triggers and accepts the event if at least one trigger is
     * satisfied
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Wrappers for TriggerRange objects that define the trigger selection
    std::vector<TriggerWithBuffer> ranges;
    
    /**
     * \brief Buffers into which decisions of MC triggers are read
     * 
     * The buffers can be shared among multiple ranges.
     */
    std::map<std::string, Bool_t> buffers;
};


template<typename C>
PECTriggerFilterMC::PECTriggerFilterMC(std::string const &name, C const &ranges_):
    PECTriggerFilter(name)
{
    ConstructRanges(ranges_);
}


template<typename C>
PECTriggerFilterMC::PECTriggerFilterMC(C const &ranges_):
    PECTriggerFilter("TriggerFilter")
{
    ConstructRanges(ranges_);
}


template<typename C>
void PECTriggerFilterMC::ConstructRanges(C const &ranges_)
{
    // A sanity check
    if (ranges_.size() == 0)
        throw std::logic_error("PECTriggerFilterMC::ConstructRanges: Provided collection of "
          "TriggerRange objects is empty.");
    
    
    // Save pointers to the TriggerRange objects
    ranges.reserve(ranges_.size());
    
    for (typename C::const_iterator it = ranges_.cbegin(); it != ranges_.cend(); ++it)
        ranges.emplace_back(&*it);
}



/// A simple short-cut to construct trigger filters for data or simulation
template<typename... Args>
PECTriggerFilter *BuildPECTriggerFilter(bool isData, Args const & ...args)
{
    if (isData)
        return new PECTriggerFilterData(args...);
    else
        return new PECTriggerFilterMC(args...);
}

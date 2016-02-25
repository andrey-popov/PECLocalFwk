#pragma once

#include <PECFwk/core/AnalysisPlugin.hpp>

#include <PECFwk/extensions/TriggerRange.hpp>

#include <Rtypes.h>

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
    PECTriggerFilter(std::string const name = "PECTriggerFilter");
    
    /// Default copy constructor
    PECTriggerFilter(PECTriggerFilter const &) = default;
    
    /// Default move constructor
    PECTriggerFilter(PECTriggerFilter &&) = default;
    
    /// Assignment operator is deleted
    PECTriggerFilter &operator=(PECTriggerFilter const &) = delete;
    
    /// Trivial destructor
    virtual ~PECTriggerFilter() noexcept;
    
public:
    /**
     * \brief Requests reading of the tree with trigger information
     * 
     * All branches of the tree are disabled.
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
 * data trigger for each event in the dataset that will be processed; if the trigger corresponding
 * to a specific event is not found in the trigger tree, an exception is thrown.
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
     * available during the lifetime of PECTriggerFilter. They are not owned by the object. The
     * trigger ranges are expected not to overlap. Otherwise the behaviour of trigger selection is
     * undefined.
     */
    template<typename C>
    PECTriggerFilterData(std::string const &name, C const &ranges);
    
    /** 
     * \brief Construct a filter with the given trigger selection
     * 
     * A short-cut for the above version with a default name "PECTriggerFilterData".
     */
    template<typename C>
    PECTriggerFilterData(C const &ranges);
    
    /// Default copy constructor
    PECTriggerFilterData(PECTriggerFilterData const &) = default;
    
    /// Default move constructor
    PECTriggerFilterData(PECTriggerFilterData &&) = default;
    
    /// Assignment operator is deleted
    PECTriggerFilterData &operator=(PECTriggerFilterData const &) = delete;
    
    /// Trivial destructor
    virtual ~PECTriggerFilterData() noexcept;
    
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
    /**
     * \brief Pointers to TriggerRange objects that define the trigger selection
     * 
     * The pointed-to objects must be available during the lifetime of PECTriggerFilter. They are
     * not owned by this class.
     */
    std::vector<TriggerRange const *> ranges;
    
    /**
     * \brief Pointer to the current TriggerRange object
     * 
     * If the current event does not match any TriggerRange object or no attempt to find the
     * corresponding range has been made yet, the poiter is null.
     */
    TriggerRange const *currentRange;
    
    /// Buffer into which decision of the selected trigger is read
    Bool_t bfAccepted;
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
    PECTriggerFilter("PECTriggerFilterData"),
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
 * \brief Implements a trigger selection for data
 * 
 * The selection is described by a collection of TriggerRange objects. All provided MC triggers
 * must be present.
 * 
 * An event passes the trigger selection if it is accepted by at least one of the MC triggers. It
 * is assigned a weight calculated as a sum of integrated luminosities for all triggers that accept
 * the event.
 */
class PECTriggerFilterMC: public PECTriggerFilter
{
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
     * A short-cut for the above version with a default name "PECTriggerFilterMC".
     */
    template<typename C>
    PECTriggerFilterMC(C const &ranges);
    
    /// Default move constructor
    PECTriggerFilterMC(PECTriggerFilterMC &&) = default;
    
    /// Assignment operator is deleted
    PECTriggerFilterMC &operator=(PECTriggerFilterMC const &) = delete;
    
    /// Destructor
    virtual ~PECTriggerFilterMC() noexcept;
    
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
    virtual double GetWeight() const;
    
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
    /**
     * \brief Non-owning pointers to TriggerRange objects that define the trigger selection and
     * buffers that store trigger decisions
     * 
     * The buffers to which pair::second point, store decisions of the corresponding MC triggers.
     * All the buffers are allocated in a block, to which the attribute 'buffer' points. Such
     * organization is preferred to using Bool_t in pair::second because the same MC trigger
     * might be specified in several trigger ranges.
     */
    std::vector<std::pair<TriggerRange const *, Bool_t *>> ranges;
    
    /**
     * \brief Buffer into which trigger decisions are read
     * 
     * Points to a dynamically allocated array. The buffers are normally accessed via pointers
     * in ranges::value_type::second. This variable is only used to initialize them and set
     * addresses of branches.
     */
    Bool_t *buffer;
};


template<typename C>
PECTriggerFilterMC::PECTriggerFilterMC(std::string const &name, C const &ranges_):
    PECTriggerFilter(name),
    buffer(nullptr)
{
    ConstructRanges(ranges_);
}


template<typename C>
PECTriggerFilterMC::PECTriggerFilterMC(C const &ranges_):
    PECTriggerFilter("PECTriggerFilterMC"),
    buffer(nullptr)
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
        ranges.emplace_back(&*it, nullptr);
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

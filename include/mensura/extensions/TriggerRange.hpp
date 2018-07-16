#pragma once

#include <mensura/core/EventID.hpp>

#include <initializer_list>
#include <set>
#include <string>


/**
 * \class TriggerRange
 * \brief An aggregate to describe selection on triggers
 * 
 * This class puts together inputs for various aspects of trigger selection in a given data-taking
 * period. The selection in data is described by a set of triggers, which are assumed to be
 * combined with an inclusive disjunction (i.e. logical OR). An integrated luminosity corresponding
 * to this set of triggers is stored. In addition, a set of triggers to be applied in simulation
 * (which are also assumed to be be combined together with an inclusive disjunction) is saved.
 * 
 * Trigger names are stored and provided by this class without the "HLT_" prefix and the postfix
 * with trigger version.
 * 
 * This class is only intended to aggregate information and simplify access to it. It does not
 * describe algorithms that exploit this information, for example, to perform the actual trigger
 * selection or to calculate an event weight to be applied to simulation.
 */
class TriggerRange
{
public:
    /// Default constructor
    TriggerRange() = default;
    
    /**
     * \brief Constructor for a typical use case
     * 
     * The data-taking period is specified by the two run numbers (both boundaries are included in
     * the range). Consult documentation for SetDataTriggers and SetMCTriggers for description of
     * the remaining parameters.
     */
    TriggerRange(EventID::RunNumber_t firstRun, EventID::RunNumber_t lastRun,
      std::initializer_list<std::string> const &dataTriggers, double intLumi,
      std::initializer_list<std::string> const &mcTriggers);
    
    /// Specialization of the above version to be used when a single trigger is requested
    TriggerRange(EventID::RunNumber_t firstRun, EventID::RunNumber_t lastRun,
      std::string const &dataTrigger, double intLumi, std::string const &mcTrigger);
    
    /// Default copy constructor
    TriggerRange(TriggerRange const &) = default;
    
    /// Default assignment operator
    TriggerRange &operator=(TriggerRange const &) = default;
    
    /// Destructor
    virtual ~TriggerRange() = default;

public:
    /**
     * \brief Returns trigger patterns defining selection in data
     * 
     * When the set contains multiple patterns, they are assumed to be combined with an inclusive
     * disjunction.
     */
    std::set<std::string> const &GetDataTriggers() const;
    
    /// Returns one of data trigger patterns
    [[deprecated("Use TriggerRange::GetDataTriggers instead")]]
    std::string const &GetDataTriggerPattern() const;
    
    /// Returns effective integrated luminosity corresponding to the trigger set, 1/pb
    double GetLuminosity() const;
    
    /**
     * \brief Returns trigger patterns defining selection to be applied to simulation
     * 
     * When the set contains multiple patterns, they are assumed to be combined with an inclusive
     * disjunction.
     */
    std::set<std::string> const &GetMCTriggers() const;
    
    /// Returns one of MC trigger patterns
    [[deprecated("Use TriggerRange::GetMCTriggers instead")]]
    std::string const &GetMCTriggerPattern() const;
    
    /**
     * \brief Checks whether the specified event is in the data-taking period described by this
     * object
     * 
     * Returns true if the specified event is in the allowed range. Both lower and upper bounds
     * are included.
     */
    bool InRange(EventID const &eventID) const;
    
    /**
     * \brief Specifies trigger selection in data and the corresponding integrated luminosity
     * 
     * The first argument is a set of trigger names. The "HLT_" prefix and version postfix are not
     * required. The second argument is the effective integrated luminosity recorded with the given
     * set of trigger (combined with an inclusive disjunction), measured in 1/pb.
     */
    void SetDataTriggers(std::initializer_list<std::string> const &dataTriggers, double intLumi);
    
    /**
     * \brief Specifies trigger selection to be applied in simulation
     * 
     * Usually, this selection should be as similar as possible to the one applied in data. The
     * provided set of triggers must meet the same requirements as the one given to method
     * SetDataTriggers.
     */
    void SetMCTriggers(std::initializer_list<std::string> const &mcTriggers);
    
    /**
     * \brief Specifies the data-taking period
     * 
     * This method sets the data range covered by this object. Both boundaries are included in the
     * range.
     */
    void SetRange(EventID const &first, EventID const &last);
    
private:
    /**
     * \brief Fills the given set with representations of provided trigger names
     * 
     * Throws an exception if a duplicate is found.
     */
    void FillTriggerSet(std::set<std::string> &target,
      std::initializer_list<std::string> const &triggerNames);
    
    /**
     * \brief Constructs a representation of the provided trigger name
     * 
     * The "HLT_" prefix and version postfix are removed if present. The postfix can take the form
     * of "_v", "_v\*", or "_v\d+".
     */
    static std::string GetTriggerBasename(std::string const &name);

private:
    /// The beginning of the EventID range (included in the range)
    EventID firstEvent;
    
    /// The end of the EventID range (included in the range)
    EventID lastEvent;
    
    /// Patterns of trigger names to be used in data
    std::set<std::string> dataTriggers;
    
    /// Integrated luminosity recorded with provided trigger set in data, 1/pb
    double intLumi;
    
    /// Patterns of trigger names to be used in simulation
    std::set<std::string> mcTriggers;
};

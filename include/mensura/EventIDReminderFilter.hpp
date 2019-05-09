#pragma once

#include <mensura/AnalysisPlugin.hpp>

#include <mensura/EventID.hpp>

#include <map>
#include <string>
#include <vector>


class EventIDReader;


/**
 * \class EventIDReminderFilter
 * \brief Filters events based on a reminder of division of their event number
 * 
 * This plugin calculates a reminder of division of the event number part of event ID by a
 * configurable denominator. If this reminder is larger that the configurable maximal allowed
 * value, the event is rejected. If needed, this logic can be inverted by setting a dedicated flag.
 * 
 * The main foreseen use case for this plugin is to define test and training samples for an MVA
 * classifier.
 * 
 * The filter relies on the presence of a EventIDReader with a default name "EventID".
 */
class EventIDReminderFilter: public AnalysisPlugin
{
public:
    /// Constructor
    EventIDReminderFilter(std::string const &name, unsigned maxReminder,
     unsigned denominator, bool isReversed = false);
    
    /// A short-cut for the above version with a default name "EventIDReminderFilter"
    EventIDReminderFilter(unsigned maxReminder, unsigned denominator, bool isReversed = false);
    
    /// Default copy constructor
    EventIDReminderFilter(EventIDReminderFilter const &) = default;
    
    /// Default move constructor
    EventIDReminderFilter(EventIDReminderFilter &&) = default;
    
    /// Assignment operator is deleted
    EventIDReminderFilter &operator=(EventIDReminderFilter const &) = delete;
    
    /// Trivial destructor
    virtual ~EventIDReminderFilter() noexcept;

public:
    /**
     * \brief Performs initialization for a new dataset
     * 
     * Reimplemented from Plugin.
     */
    virtual void BeginRun(Dataset const &) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Plugin.
     */
    virtual Plugin *Clone() const override;
    
    /// Changes name of the plugin that provides event ID
    void SetEventIDPluginName(std::string const &name);
    
private:
    /**
     * \brief Performs selection based on event number
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;

private:
    /// Name of the plugin that provides access to event ID
    std::string eventIDPluginName;
    
    /// Non-owning pointer to the plugin that provides access to event ID
    EventIDReader const *eventIDPlugin;
    
    /**
     * \brief Maximal allowed reminder of a division of an event number by the denominator
     * 
     * If the reminder is larger, the event is rejected.
     */
    unsigned maxReminder;
    
    /// Denominator to divide the event number
    unsigned denominator;
    
    /// Indicates if the logic of the filter should be reversed
    bool isReversed;
};

/**
 * \file FilterEventIDReminderPlugin.hpp
 * 
 * Defines a class that filters event based on based on a reminder of division of their event number
 * by a specified denominator.
 */

#pragma once

#include <Plugin.hpp>

#include <PECReaderPlugin.hpp>
#include <EventID.hpp>

#include <string>
#include <map>
#include <vector>


/**
 * \class FilterEventIDReminderPlugin
 * \brief Filters events on a reminder of division of their event number
 * 
 * The plugin calculates a reminder of division of the event number part of event ID by a
 * configurable denominator. If this reminder is larger that the configurable maximal allowed value,
 * the event is rejected. If needed, this logic can be inverted by setting a dedicated flag.
 */
class FilterEventIDReminderPlugin: public Plugin
{
public:
    /**
     * \brief Constructor
     * 
     * Consult the general documentation of the class for details.
     */
    FilterEventIDReminderPlugin(std::string const &name, unsigned maxReminder,
     unsigned denominator, bool isReversed = false);

public:
    /**
     * \brief Creates a newly-initialised copy
     * 
     * Consult documentation of the overriden method for details.
     */
    Plugin *Clone() const;
    
    /**
     * \brief Notifies this that a dataset has been opened
     * 
     * Consult documentation of the overriden method for details.
     */
    void BeginRun(Dataset const &dataset);
    
    /**
     * \brief Notifies this that a dataset has been closed
     * 
     * Consult documentation of the overriden method for details.
     */
    void EndRun();
    
    /**
     * \brief Decides if the event should be kept or rejected based on its event number
     * 
     * Consult documentation of the overriden method for details.
     */
    bool ProcessEvent();

private:
    /// Pointer to PECReaderPlugin
    PECReaderPlugin const *reader;
    
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

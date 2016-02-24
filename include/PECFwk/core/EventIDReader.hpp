#pragma once

#include <PECFwk/core/ReaderPlugin.hpp>

#include <PECFwk/core/EventID.hpp>

#include <vector>


/**
 * \class EventIDReader
 * \brief An abstract base class for a reader plugin that provides event ID
 */
class EventIDReader: public ReaderPlugin
{
public:
    /// Creates plugin with the given name
    EventIDReader(std::string const name = "EventID");
    
    /// Default copy constructor
    EventIDReader(EventIDReader const &) = default;
    
    /// Default move constructor
    EventIDReader(EventIDReader &&) = default;
    
    /// Default assignment operator
    EventIDReader &operator=(EventIDReader const &) = default;
    
    /// Trivial destructor
    virtual ~EventIDReader() noexcept;
    
public:
    /// Returns ID of the current event
    EventID const &GetEventID() const;
    
protected:
    /// ID of the current event converted into the standard format of the framework
    EventID eventID;
};

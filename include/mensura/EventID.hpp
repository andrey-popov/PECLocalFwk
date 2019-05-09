/**
 * \file EventID.hpp
 * 
 * The module defines an aggregate to store the event ID information.
 */

#pragma once


/**
 * \class EventID
 * \brief The class aggregates event ID information
 */
class EventID
{
public:
    using RunNumber_t = unsigned long;
    using LumiBlockNumber_t = unsigned long;
    using EventNumber_t = unsigned long long;
    using BXNumber_t = unsigned;
    
public:
    /// Default constructor
    EventID();
    
    /// Constructor to specify the ID
    EventID(RunNumber_t runNumber, LumiBlockNumber_t lumiBlockNumber, EventNumber_t eventNumber,
      BXNumber_t bxNumber = 0);
    
    /**
     * \brief Constructor from run number solely
     * 
     * Consult the documentation for method Set(unsigned long, bool) for the description.
     */
    EventID(RunNumber_t runNumber, bool minimal = true);
    
    /// Default copy constructor
    EventID(EventID const &) = default;
    
    /// Default assignment operator
    EventID &operator=(EventID const &) = default;

public:
    /// Sets the ID
    void Set(RunNumber_t runNumber, LumiBlockNumber_t lumiBlockNumber, EventNumber_t eventNumber,
      BXNumber_t bxNumber = 0);
    
    /**
     * \brief Sets the ID with the run number only
     * 
     * Sets the specified run number. If the second argument is true (default), the
     * lumiBlockNumber and eventNumber are set to 0, hence this instance of EventID is
     * guaranteed to be smaller or equal than all the events with the same run number. Otherwise
     * lumiBlockNumber and eventNumber are set to (-1), hence this event is greater than any
     * one with the same run number. This method is useful if the user wants to specify some
     * range based on the run numbers only.
     */
    void Set(RunNumber_t runNumber, bool minimal = true);
    
    /// Comparison operator
    bool operator<(EventID const &rhs) const;
    
    /// Equality operator
    bool operator==(EventID const &rhs) const;
    
    /// Unstrict comparison operator
    bool operator<=(EventID const &rhs) const;
    
    /// Get the run number
    RunNumber_t Run() const;
    
    /// Get the luminosity block number
    LumiBlockNumber_t LumiBlock() const;
    
    /// Get the event number
    EventNumber_t Event() const;
    
    /**
     * \brief Returns bunch crossing number
     * 
     * In case of simulation or if the bunch crossing number was not provided in the first place,
     * returns zero.
     */
    BXNumber_t BunchCrossing() const;

private:
    RunNumber_t runNumber;  ///< The run number
    LumiBlockNumber_t lumiBlockNumber;  ///< The luminosity block number
    EventNumber_t eventNumber;  ///< The event number
    BXNumber_t bxNumber;  ///< Bunch crossing number
};

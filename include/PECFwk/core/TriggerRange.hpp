/**
 * \file TriggerRange.hpp
 * 
 * The module defines a class to store the full information about trigger selection to be used with
 * a specific fraction of data.
 */

#pragma once

#include <PECFwk/core/EventID.hpp>
#include <PECFwk/core/PECReaderForward.hpp>

#include <string>
#include <functional>
#include <list>


/**
 * \class TriggerRange
 * \brief An aggregate to store trigger-related information
 * 
 * This class puts together relevant information for a trigger selection in a given data-taking
 * period. In addition to the name of the trigger used in specified EventID range in data and
 * the corresponding integrated luminosity, it contains the name of the corresponding MC trigger
 * and an optional modification of the offline event selection (useful if some triggers have higher
 * thresholds, for example).
 * 
 * The class is used solely to aggregate the information together and to simply access to it, it
 * shall not describe algorithms to exploit this information, for example, to perform the actual
 * trigger selection or to calculate the MC event weight due to the triggers.
 * 
 * The class is copyable.
 */
class TriggerRange
{
public:
    /// The default constructor
    TriggerRange() = default;
    
    /**
     * \brief Constructor
     * 
     * Constructor for the most typical use case. The data range is specified by the two run
     * number (both boundaries are included in the range). Consult documentation for
     * SetDataTrigger and SetMCTrigger for description of the rest parameters.
     */
    TriggerRange(unsigned long firstRun, unsigned long lastRun,
     std::string const &dataTriggerPattern, double intLumi,
     std::string const &MCTriggerPattern);
    
    /// Default copy constructor
    TriggerRange(TriggerRange const &) = default;
    
    /// Destructor
    virtual ~TriggerRange() = default;
    
    /// Default assignment operator
    TriggerRange &operator=(TriggerRange const &) = default;

public:
    /**
     * \brief Specifies the data range
     * 
     * The method sets the data range which this object is to address. Both boundaries are
     * included in the range.
     */
    void SetRange(EventID const &first, EventID const &last);
    
    /**
     * \brief Specifies the trigger in data and the corresponding int. luminosity
     * 
     * The first argument is the full trigger name; however, the "HLT_" prefix and version
     * postfix might optionally be stripped. Internally it is fed into the GetTriggerBasename
     * method and the result is stored.
     * 
     * The integrated luminosity is the effective integrated luminosity recorded with the given
     * trigger, measured in 1/pb.
     */
    void SetDataTrigger(std::string const &pattern, double intLumi);
    
    /**
     * \brief Specifies the trigger to be used in MC
     * 
     * Specifies the pattern for the name of the trigger in MC, which corresponds to the
     * specified trigger in data (i.e., normally, they should be as similar as possible). It
     * must meet the same requirements as the pattern provided to SetDataTrigger method.
     */
    void SetMCTrigger(std::string const &pattern);
    
    /**
     * \brief Sets the additional event selection
     * 
     * Specifies the additional offline event selection to be used along with the provided
     * data trigger. It is to be applied in PECReader after the general event selection and is
     * useful when triggers with different thresholds are used for different data-taking
     * ranges. The argument can be pointer to a function, a functor object, or a lambda
     * expression; the callable object should return true if the event passes the selection.
     */
    void SetEventSelection(std::function<bool(PECReader const &)> eventSelection);
    
    /**
     * \brief Checks whether the specified event is in the range
     * 
     * Returns true if the specified event is in the allowed range. Both lower and upper bounds
     * are included in the range.
     */
    bool InRange(EventID const &eventID) const;
    
    /**
     * \brief Performs the additional event selection
     * 
     * Performs the addtional offline event selection as specified by SetEventSelection method.
     * It is useful, e.g., if triggers with different thresholds are used over the data-taking
     * period. If SetEventSelection method has never been called, this function returns true
     * for any event. Apart from providing a callable object via SetEventSelection, the user
     * can implement the event selection in a derived class by overriding this virtual method.
     */
    virtual bool PassEventSelection(PECReader const &reader) const;
    
    /// Returns the trigger pattern used in data
    std::string const &GetDataTriggerPattern() const;
    
    /// Returns the trigger pattern to be asked for in MC
    std::string const &GetMCTriggerPattern() const;
    
    /// Returns the effective integrated luminosity, 1/pb
    double GetLuminosity() const;
    
    /**
     * \brief Removes the "HLT_" prefix and version postfix from trigger name
     * 
     * The postfix might take the form of "_v", "_v\*", or "_v\d+". Both the postfix and
     * prefix are optional.
     */
    static std::string GetTriggerBasename(std::string const &name);

private:
    EventID firstEvent;  ///< The beginning of the EventID range (included in the range)
    EventID lastEvent; ///< The end of the EventID range (included in the range)
    /// Pattern for the name of the trigger used in data
    std::string dataTriggerPattern;
    double intLumi;  ///< Integrated luminosity recorded with this trigger in data, 1/pb
    /// Pattern for the name of the trigger to be asked for in MC
    std::string MCTriggerPattern;
    /// An additional offline event selection
    std::function<bool(PECReader const &)> eventSelection;
    
    //TODO: Add an object and configuration to access the trigger scale factors. Though the
    //object should probably be stored in the TriggerSelectionInterface (or derieved) and it's
    //common to all the ranges; to pick up the specific SF the two trigger masks can be used.
};

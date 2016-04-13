#pragma once

#include <mensura/core/EventID.hpp>

#include <string>
#include <functional>
#include <list>


/**
 * \class TriggerRange
 * \brief An aggregate to store trigger-related information
 * 
 * This class puts together relevant information for a trigger selection in a given data-taking
 * period. In addition to the name of the trigger used in specified EventID range in data and
 * the corresponding integrated luminosity, it contains the name of the corresponding MC trigger.
 * 
 * The class is used solely to aggregate the information together and to simplify access to it. It
 * does not describe algorithms that exploit this information, for example, to perform the actual
 * trigger selection or to calculate the MC event weight due to the triggers.
 */
class TriggerRange
{
public:
    /// Default constructor
    TriggerRange() = default;
    
    /**
     * \brief Constructor
     * 
     * Constructor for the most typical use case. The data range is specified by the two run
     * number (both boundaries are included in the range). Consult documentation for
     * SetDataTrigger and SetMCTrigger for description of the remaining parameters.
     */
    TriggerRange(unsigned long firstRun, unsigned long lastRun,
      std::string const &dataTriggerPattern, double intLumi,
      std::string const &mcTriggerPattern);
    
    /// Default copy constructor
    TriggerRange(TriggerRange const &) = default;
    
    /// Default assignment operator
    TriggerRange &operator=(TriggerRange const &) = default;
    
    /// Destructor
    virtual ~TriggerRange() = default;

public:
    /**
     * \brief Checks whether the specified event is in the range
     * 
     * Returns true if the specified event is in the allowed range. Both lower and upper bounds
     * are included in the range.
     */
    bool InRange(EventID const &eventID) const;
    
    /// Returns the trigger pattern used in data
    std::string const &GetDataTriggerPattern() const;
    
    /// Returns the effective integrated luminosity, 1/pb
    double GetLuminosity() const;
    
    /// Returns the trigger pattern to be asked for in MC
    std::string const &GetMCTriggerPattern() const;
    
    /**
     * \brief Specifies the trigger in data and the corresponding int. luminosity
     * 
     * The first argument is the full trigger name; however, the "HLT_" prefix and version
     * postfix are not required. Internally it is fed into the GetTriggerBasename method and the
     * result is stored.
     * 
     * The integrated luminosity is the effective integrated luminosity recorded with the given
     * trigger, measured in 1/pb.
     */
    void SetDataTrigger(std::string const &pattern, double intLumi);
    
    /**
     * \brief Specifies the trigger to be used in MC
     * 
     * Specifies the pattern for the name of the trigger in MC, which corresponds to the
     * specified trigger in data (normally, they should be as similar as possible). The string
     * meet the same requirements as the pattern provided to SetDataTrigger method.
     */
    void SetMCTrigger(std::string const &pattern);
    
    /**
     * \brief Specifies the data range
     * 
     * The method sets the data range which this object is to address. Both boundaries are
     * included in the range.
     */
    void SetRange(EventID const &first, EventID const &last);
    
private:
    /**
     * \brief Removes the "HLT_" prefix and version postfix from trigger name
     * 
     * The postfix might take the form of "_v", "_v\*", or "_v\d+". Both the postfix and
     * prefix are optional.
     */
    static std::string GetTriggerBasename(std::string const &name);

private:
    /// The beginning of the EventID range (included in the range)
    EventID firstEvent;
    
    /// The end of the EventID range (included in the range)
    EventID lastEvent;
    
    /// Pattern for the name of the trigger used in data
    std::string dataTriggerPattern;
    
    /// Integrated luminosity recorded with this trigger in data, 1/pb
    double intLumi;
    
    /// Pattern for the name of the trigger to be asked for in MC
    std::string mcTriggerPattern;
};

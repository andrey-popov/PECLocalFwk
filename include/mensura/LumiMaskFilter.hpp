#pragma once

#include <mensura/AnalysisPlugin.hpp>

#include <mensura/EventID.hpp>

#include <istream>
#include <map>
#include <string>
#include <utility>
#include <vector>


class EventIDReader;


/**
 * \class LumiMaskFilter
 * \brief Filters events based on a luminosity certification mask
 * 
 * This plugin checks events against a luminosity mask and, depending on the configuration, either
 * keeps or rejects events that are comparible with the mask. The mask is provided in the standard
 * JSON format.
 * 
 * The filter relies on the presence of a EventIDReader with a default name "EventID".
 */
class LumiMaskFilter: public AnalysisPlugin
{
private:
    /**
     * \brief Type to describe a range of consecutive luminosity sections
     * 
     * Boundaries are included in the range.
     */
    typedef std::pair<unsigned long, unsigned long> lumiRange_t;
    
public:
    /**
     * \brief Creates a plugin with the given name
     * 
     * The luminosity mask must be provided in the standard JSON format. An exception will be
     * thrown if hte file cannot be parsed.
     * 
     * By default, this plugin rejects events that are compatible with the mask. This logic can be
     * inverted using the last argument.
     */
    LumiMaskFilter(std::string const &name, std::string const &lumiMaskFileName,
     bool rejectKnownEvent = true);
    
    /// A short-cut for the above version with a default name "LumiMaskFilter"
    LumiMaskFilter(std::string const &lumiMaskFileName, bool rejectKnownEvent = true);
    
    /// Default copy constructor
    LumiMaskFilter(LumiMaskFilter const &) = default;
    
    /// Default move constructor
    LumiMaskFilter(LumiMaskFilter &&) = default;
    
    /// Assignment operator is deleted
    LumiMaskFilter &operator=(LumiMaskFilter const &) = delete;
    
    /// Trivial destructor
    virtual ~LumiMaskFilter() noexcept;

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
    /// Reads JSON file with luminosity mask
    void LoadLumiMask(std::string const &lumiMaskFileName);
    
    /**
     * \brief Checks the current event against the luminosity mask
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
    /// Reads next non-blank character from the stream
    static char GetNonBlank(std::istream &input);

private:
    /// Name of the plugin that provides access to event ID
    std::string eventIDPluginName;
    
    /// Non-owning pointer to the plugin that provides access to event ID
    EventIDReader const *eventIDPlugin;
    
    /**
     * \brief Switch defining the logic of filtering
     * 
     * If set to true, the plugin rejects events are compatible with the provided mask, and vice
     * versa.
     */
    bool rejectKnownEvent;
    
    /**
     * \brief Luminosity mask for filtering
     * 
     * The key of the map is the run number. Entries for each run are ordered.
     */
    std::map<unsigned long, std::vector<lumiRange_t>> lumiMask;
};

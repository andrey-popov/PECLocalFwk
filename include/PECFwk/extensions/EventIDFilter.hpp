#pragma once

#include <PECFwk/core/AnalysisPlugin.hpp>

#include <PECFwk/core/EventID.hpp>

#include <map>
#include <string>
#include <vector>


class EventIDReader;


/**
 * \class EventIDFilter
 * \brief Filters events based on their ID
 * 
 * Theis plugin filters events based on their ID (run, lumi, event numbers). The user specifies a
 * list of event IDs in a text file, and the plugin either keeps or rejects events whose IDs are
 * found in the list.
 * 
 * The filter relies on the presence of a EventIDReader with a default name "EventID".
 * 
 * \warning The class can filter atomic datasets only (i.e. ones that contain a single file).
 */
class EventIDFilter: public AnalysisPlugin
{
public:
    /// Constructor
    EventIDFilter(std::string const &name, std::string const &eventIDsFileName,
     bool rejectKnownEvent = true);
    
    /// A short-cut for the above version with a default name "EventIDFilter"
    EventIDFilter(std::string const &eventIDsFileName, bool rejectKnownEvent = true);
    
    /// Default move constructor
    EventIDFilter(EventIDFilter &&) = default;
    
    /// Assignment operator is deleted
    EventIDFilter &operator=(EventIDFilter const &) = delete;
    
    /// Trivial destructor
    virtual ~EventIDFilter() noexcept;

private:
    /// Copy constructor used in method Clone
    EventIDFilter(EventIDFilter const &src);

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
    /// Reads event ID lists from the given text file
    void LoadEventIDLists(std::string const &eventIDsFileName);
    
    /**
     * \brief Checks jets in the current event and performs the selection
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
     * \brief Switch defining the logic of filtering
     * 
     * If set to true, the plugin rejects events whose IDs are mentioned in the provided file.
     * If set to false, it keeps only events with IDs written in the file.
     */
    bool rejectKnownEvent;
    
    /// Map to store event IDs. The key of the map is a short name of corresponding input file
    std::map<std::string, std::vector<EventID>> eventIDsAllFiles;
    
    /// Pointer to vector of event IDs for the current ROOT file (note it might be null)
    std::vector<EventID> const *eventIDsCurFile;
};

#pragma once

#include <mensura/AnalysisPlugin.hpp>

#include <mensura/EventID.hpp>

#include <istream>
#include <map>
#include <memory>
#include <string>
#include <vector>


class EventIDReader;


/**
 * \class EventIDFilter
 * \brief Filters events based on their ID
 * 
 * This plugin filters events based on their ID (run, lumi, event numbers). The user specifies a
 * list of event IDs in a text file, and the plugin either keeps or rejects events whose IDs are
 * found in the list.
 * 
 * The file should have the following format. First, it identifies the dataset to which provided
 * event IDs correpond. This is done with line "Dataset: <ID>", where by default <ID> is the unique
 * label of the source dataset, as given by Dataset::GetSourceDatasetID(). Alternatively, the
 * plugin can be configured to use name of the first file in the dataset, see documentation for
 * method SetUseFileName. After the dataset ID, a list of event IDs is provided in the format
 * <run>:<lumi>:<event>, one per line. Entries for as many datasets as needed can be provided in
 * the input file. Empty lines can be inserted anywhere. Comments starting with symbol '#' are
 * ignored.
 * 
 * The filter relies on the presence of a EventIDReader with a default name "InputData".
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
    
private:
    /// Default copy constructor used in method Clone
    EventIDFilter(EventIDFilter const &) = default;
    
public:
    /**
     * \brief Performs initialization for a new dataset
     * 
     * Reimplemented from Plugin.
     */
    virtual void BeginRun(Dataset const &dataset) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Plugin.
     */
    virtual Plugin *Clone() const override;
    
    /// Changes name of the plugin that provides event ID
    void SetEventIDPluginName(std::string const &name);
    
    /**
     * \brief Switches between identifying the event list by source dataset ID or file name
     * 
     * If called with the argument set to true, the plugin will identify the event list to be used
     * using the name of the first file in the dataset (with the directory path stripped) instead
     * of the source dataset ID. The default behaviour of using the source dataset ID can be
     * restored by calling this method with the argument set to false.
     */
    void SetUseFileName(bool on = true);
    
private:
    /**
     * \brief Reads event ID lists from the given text file
     * 
     * The format of the file is described in the documentation for the class.
     */
    void LoadEventIDLists(std::string const &eventIDsFileName);
    
    /**
     * \brief Checks ID of the current event
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
    /**
     * \brief Reads next clean non-empty line from the input stream
     * 
     * Reads next line from the stream and strips off comment that starts with symbol '#'. If the
     * resulting string has zero length or consists of spaces only, it is skipped, and the next
     * line is read until a non-empty one is found. If there are no more line remaining in the
     * stream, an empty line is copied into the buffer.
     */
    static void ReadCleanLine(std::istream &input, std::string &buffer);
    
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
    
    /// Instructs to use name of the current file instead of the source dataset ID
    bool useFileName;
    
    /// Map to store event IDs. The key of the map is a short name of corresponding input file
    std::shared_ptr<std::map<std::string, std::vector<EventID>>> eventIDsAllFiles;
    
    /// Pointer to vector of event IDs for the current ROOT file (note it might be null)
    std::vector<EventID> const *eventIDsCurFile;
};

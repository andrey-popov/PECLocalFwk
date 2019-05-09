#include <mensura/EventIDFilter.hpp>

#include <mensura/EventIDReader.hpp>
#include <mensura/Processor.hpp>

#include <algorithm>
#include <fstream>
#include <regex>
#include <sstream>
#include <stdexcept>


using namespace std::literals::string_literals;


EventIDFilter::EventIDFilter(std::string const &name, std::string const &eventIDsFileName,
  bool rejectKnownEvent_ /*= true*/):
    AnalysisPlugin(name),
    eventIDPluginName("InputData"), eventIDPlugin(nullptr),
    rejectKnownEvent(rejectKnownEvent_),
    useFileName(false),
    eventIDsAllFiles(new decltype(eventIDsAllFiles)::element_type),
    eventIDsCurFile(nullptr)
{
    LoadEventIDLists(eventIDsFileName);
}


EventIDFilter::EventIDFilter(std::string const &eventIDsFileName,
  bool rejectKnownEvent_ /*= true*/):
    EventIDFilter("EventIDFilter", eventIDsFileName, rejectKnownEvent_)
{}


void EventIDFilter::BeginRun(Dataset const &dataset)
{
    // Lists of event IDs are provided on per-file basis. Since the plugin is not notified when
    //a new file in the current dataset is started, it makes sense only to filer atomic datasets
    if (dataset.GetFiles().size() != 1)
        throw std::logic_error("EventIDFilter::BeginRun: The plugin can filter atomic "
          "datasets only.");
    
    
    // Save pointer to plugin that produces jets
    eventIDPlugin = dynamic_cast<EventIDReader const *>(GetDependencyPlugin(eventIDPluginName));
    
    
    // Make a short-cut for list of event IDs for the new atomic dataset
    std::string datasetID;
    
    if (useFileName)
    {
        std::string const &fileName = dataset.GetFiles().front().name;
        datasetID = fileName.substr(fileName.find_last_of('/') + 1);
    }
    else
        datasetID = dataset.GetSourceDatasetID();
    
    auto const resIt = eventIDsAllFiles->find(datasetID);
    eventIDsCurFile = (resIt == eventIDsAllFiles->end()) ? nullptr : &resIt->second;
}


Plugin *EventIDFilter::Clone() const
{
    return new EventIDFilter(*this);
}


void EventIDFilter::SetEventIDPluginName(std::string const &name)
{
    eventIDPluginName = name;
}


void EventIDFilter::SetUseFileName(bool on /*= true*/)
{
    useFileName = on;
}


void EventIDFilter::LoadEventIDLists(std::string const &eventIDsFileName)
{
    using std::getline;
    
    
    // Open the text file with event IDs
    std::ifstream eventIDsFile(eventIDsFileName);
    
    if (not eventIDsFile.good())
    {
        std::ostringstream message;
        message << "EventIDFilter[\"" << GetName() <<
          "\"]::LoadEventIDLists: Cannot open file \"" << eventIDsFileName << "\".";
        throw std::runtime_error(message.str());
    }
    
    
    // Regular expressions to parse the file
    std::regex datasetRegex("^[ \t]*Dataset:[ \t]*([^ \t]+)[ \t]*$", std::regex::extended);
    std::regex eventIDRegex("^[ \t]*([0-9]+):([0-9]+):([0-9]+)[ \t]*$", std::regex::extended);
    std::smatch match;
    
    
    // Read IDs from the file
    std::string line;
    
    // The first non-trivial line in the group. It must contain ID of the first dataset
    ReadCleanLine(eventIDsFile, line);
    
    while (true)
    {
        // Stop the loop if there are no more lines to read
        if (line == "")
            break;
        
        
        // Extract the dataset ID from the last read line
        if (not std::regex_match(line, match, datasetRegex))
        {
            std::ostringstream message;
            message << "EventIDFilter[\"" << GetName() <<
              "\"]::LoadEventIDLists: Failed to parse file \"" << eventIDsFileName << "\".";
            throw std::runtime_error(message.str());
        }
        
        std::string const datasetID(match[1]);
        
        
        // Read event IDs
        auto &eventIDsCurFile = (*eventIDsAllFiles)[datasetID];
        
        while (true)
        {
            ReadCleanLine(eventIDsFile, line);
            
            // Stop if the end of file has been reached
            if (line == "")
                break;
            
            
            // Try to parse the line as an event ID. However, it might also be the name of the
            //next dataset
            if (std::regex_match(line, match, eventIDRegex))
                eventIDsCurFile.emplace_back(std::stoull(match[1]), std::stoull(match[2]),
                  std::stoull(match[3]));
            else
                break;
        }
        
        // Sort event ID
        std::sort(eventIDsCurFile.begin(), eventIDsCurFile.end());
    }
    
    eventIDsFile.close();
}


bool EventIDFilter::ProcessEvent()
{
    // Check if there is a list of event IDs for the current ROOT file
    if (eventIDsCurFile == nullptr)
        return not rejectKnownEvent;
    
    // Search for ID of the current event in the vector
    auto const &id = eventIDPlugin->GetEventID();
    bool const eventFound = std::binary_search(eventIDsCurFile->begin(), eventIDsCurFile->end(),
      EventID(id.Run(), id.LumiBlock(), id.Event()));
    
    
    return (rejectKnownEvent) ? not eventFound : eventFound;
}


void EventIDFilter::ReadCleanLine(std::istream &input, std::string &buffer)
{
    while (true)
    {
        // Read next raw line
        std::getline(input, buffer);
        
        if (input.eof())
        {
            buffer = "";
            return;
        }
        
        
        // Strip off comment if any
        auto pos = buffer.find_first_of('#');
        buffer = buffer.substr(0, pos);
        
        
        // Check if the resulting line is not empty or trivial.
        pos = buffer.find_first_not_of(" \t");
        
        if (pos != std::string::npos)
        {
            // The line is not empty. Terminate the loop
            return;
        }
    }
}

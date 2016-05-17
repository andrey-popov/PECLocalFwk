#include <mensura/extensions/EventIDFilter.hpp>

#include <mensura/core/EventIDReader.hpp>
#include <mensura/core/Processor.hpp>

#include <algorithm>
#include <fstream>
#include <sstream>
#include <stdexcept>


using namespace std::literals::string_literals;


EventIDFilter::EventIDFilter(std::string const &name, std::string const &eventIDsFileName,
  bool rejectKnownEvent_ /*= true*/):
    AnalysisPlugin(name),
    eventIDPluginName("InputData"), eventIDPlugin(nullptr),
    rejectKnownEvent(rejectKnownEvent_),
    eventIDsCurFile(nullptr)
{
    LoadEventIDLists(eventIDsFileName);
}


EventIDFilter::EventIDFilter(std::string const &eventIDsFileName,
  bool rejectKnownEvent_ /*= true*/):
    AnalysisPlugin("EventIDFilter"),
    eventIDPluginName("InputData"), eventIDPlugin(nullptr),
    rejectKnownEvent(rejectKnownEvent_),
    eventIDsCurFile(nullptr)
{
    LoadEventIDLists(eventIDsFileName);
}


EventIDFilter::EventIDFilter(EventIDFilter const &src):
    AnalysisPlugin(src),
    eventIDPluginName(src.eventIDPluginName), eventIDPlugin(nullptr),
    rejectKnownEvent(src.rejectKnownEvent),
    eventIDsAllFiles(),
    eventIDsCurFile(nullptr)
{}


EventIDFilter::~EventIDFilter()
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
    std::string const fileName(dataset.GetFiles().front().name);
    std::string const shortFileName = fileName.substr(fileName.find_last_of('/') + 1);
    
    auto const resIt = eventIDsAllFiles.find(shortFileName);
    eventIDsCurFile = (resIt == eventIDsAllFiles.end()) ? nullptr : &resIt->second;
}


Plugin *EventIDFilter::Clone() const
{
    return new EventIDFilter(*this);
}


void EventIDFilter::SetEventIDPluginName(std::string const &name)
{
    eventIDPluginName = name;
}


void EventIDFilter::LoadEventIDLists(std::string const &eventIDsFileName)
{
    using std::getline;
    
    
    // Open the text file with event IDs
    std::ifstream eventIDsFile(eventIDsFileName);
    
    if (not eventIDsFile.good())
        throw std::runtime_error("EventIDFilter::EventIDFilter: Cannot open file \""s +
          eventIDsFileName + "\".");
    
    
    // Read IDs from the file
    std::string line;
    bool stop = false;
    
    while (not stop)
    {
        // Read until a file name is found
        while (true)
        {
            getline(eventIDsFile, line);
            
            if (line.find("# Name of the file") != std::string::npos)
                break;
            
            if (eventIDsFile.eof())  // no more files mentioned in eventIDsFile
            {
                stop = true;
                break;
            }
        }
        
        if (stop)
            // Skip the rest of the loop
            continue;
        
        // The next line is the file name
        getline(eventIDsFile, line);
        auto &eventIDsCurFile = eventIDsAllFiles[line];
        
        
        // Skip two lines and read the number of events
        for (unsigned i = 0; i < 3; ++i)
            getline(eventIDsFile, line);
        
        std::istringstream ist(line);
        unsigned long nEntries;
        ist >> nEntries;
        
        eventIDsCurFile.reserve(nEntries);
        
        
        // Skip two lines
        getline(eventIDsFile, line);
        getline(eventIDsFile, line);
        
        
        // Read IDs
        while (true)
        {
            getline(eventIDsFile, line);
            
            if (eventIDsFile.eof() or line.length() == 0)
                break;
            
            ist.clear();
            ist.str(line);  // should contain three numbers separated by semicolon
            
            unsigned long run, lumiSection, event;
            ist >> run;
            ist.ignore();
            ist >> lumiSection;
            ist.ignore();
            ist >> event;
            
            eventIDsCurFile.emplace_back(run, lumiSection, event);
        }
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
    bool const eventFound = (std::find(eventIDsCurFile->begin(), eventIDsCurFile->end(),
      EventID(id.Run(), id.LumiBlock(), id.Event())) != eventIDsCurFile->end());
    
    
    return (rejectKnownEvent) ? not eventFound : eventFound;
}

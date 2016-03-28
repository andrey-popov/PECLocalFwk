#include <PECFwk/extensions/LumiMaskFilter.hpp>

#include <PECFwk/core/EventIDReader.hpp>
#include <PECFwk/core/Processor.hpp>

#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>


using namespace std::literals::string_literals;


LumiMaskFilter::LumiMaskFilter(std::string const &name, std::string const &lumiMaskFileName,
  bool rejectKnownEvent_ /*= true*/):
    AnalysisPlugin(name),
    eventIDPluginName("EventID"), eventIDPlugin(nullptr),
    rejectKnownEvent(rejectKnownEvent_)
{
    LoadLumiMask(lumiMaskFileName);
}


LumiMaskFilter::LumiMaskFilter(std::string const &lumiMaskFileName,
  bool rejectKnownEvent_ /*= true*/):
    AnalysisPlugin("LumiMaskFilter"),
    eventIDPluginName("EventID"), eventIDPlugin(nullptr),
    rejectKnownEvent(rejectKnownEvent_)
{
    LoadLumiMask(lumiMaskFileName);
}


LumiMaskFilter::~LumiMaskFilter()
{}


void LumiMaskFilter::BeginRun(Dataset const &)
{
    // Save pointer to plugin that produces jets
    eventIDPlugin = dynamic_cast<EventIDReader const *>(
      GetMaster().GetPluginBefore(eventIDPluginName, GetName()));
}


Plugin *LumiMaskFilter::Clone() const
{
    return new LumiMaskFilter(*this);
}


void LumiMaskFilter::SetEventIDPluginName(std::string const &name)
{
    eventIDPluginName = name;
}


void LumiMaskFilter::LoadLumiMask(std::string const &lumiMaskFileName)
{
    using namespace std;
    
    // Open the JSON file
    ifstream lumiMaskFile(lumiMaskFileName);
    
    
    // The JSON file starts with a brace
    if (GetNonBlank(lumiMaskFile) != '{')
        throw runtime_error("LumiMaskFilter::LoadLumiMask: File \""s + lumiMaskFileName +
          "\" is not a valide JSON file because the opening brace is missing.");
    
    
    // Read runs one by one
    stringstream word;
    string lastKeyRead("(none)");
    
    while (true)
    {
        if (GetNonBlank(lumiMaskFile) != '"')
            throw runtime_error("LumiMaskFilter::LoadLumiMask: File \""s + lumiMaskFileName +
              "\" is not a valide JSON file because a key is formatted incorrectly. Last key read "
              "successfully is \"" + lastKeyRead + "\".");
        
        
        // Read the run number
        word.str("");
        word.clear();
        char c;
        
        while (true)
        {
            c = GetNonBlank(lumiMaskFile);
            
            if (c < '0' or c > '9')  // Not a digit
                break;
            else
                word << c;
        }
        
        if (c != '"')
            throw runtime_error("LumiMaskFilter::LoadLumiMask: In file \""s + lumiMaskFileName +
              "\" a key  with run number is formatted incorrectly. Last key read successfully "
              "is \"" + lastKeyRead + "\".");
        
        lastKeyRead = word.str();
        unsigned long runNumber;
        word >> runNumber;
        auto &lumiRanges = lumiMask[runNumber];
        
        
        // After the key there should be a colon
        if (GetNonBlank(lumiMaskFile) != ':')
            throw runtime_error("LumiMaskFilter::LoadLumiMask: File \""s + lumiMaskFileName +
              "\" is not a valide JSON file because the colon after key \"" + lastKeyRead +
              "\" is missing.");
        
        
        // Now a list of lumi ranges should start
        if (GetNonBlank(lumiMaskFile) != '[')
            throw runtime_error("LumiMaskFilter::LoadLumiMask: In file \""s + lumiMaskFileName +
              "\" list of luminosity ranges for key \"" + lastKeyRead + "\" is of an illegal "
              "format.");
        
        
        // Read the list of lumi ranges
        while (true)
        {
            if (GetNonBlank(lumiMaskFile) != '[')
                throw runtime_error("LumiMaskFilter::LoadLumiMask: In file \""s +
                  lumiMaskFileName + "\" list of luminosity ranges for key \"" + lastKeyRead +
                  "\" is of an illegal format.");
            
            
            // First lumi section in the range
            word.str("");
            word.clear();
            
            while (true)
            {
                c = GetNonBlank(lumiMaskFile);
                
                if (c < '0' or c > '9')  // Not a digit
                    break;
                else
                    word << c;
            }
            
            if (c != ',')  // Must be the comma separating the lumi section numbers in a range
                throw runtime_error("LumiMaskFilter::LoadLumiMask: In file \""s +
                  lumiMaskFileName + "\" list of luminosity ranges for key \"" + lastKeyRead +
                  "\" is of an illegal format.");
            
            unsigned long lumiStart;
            word >> lumiStart;
            
            
            // Last lumi section in the range
            word.str("");
            word.clear();
            
            while (true)
            {
                c = GetNonBlank(lumiMaskFile);
                
                if (c < '0' or c > '9')  // Not a digit
                    break;
                else
                    word << c;
            }
            
            if (c != ']')  // End of the range
                throw runtime_error("LumiMaskFilter::LoadLumiMask: In file \""s +
                  lumiMaskFileName + "\" list of luminosity ranges for key \"" + lastKeyRead +
                  "\" is of an illegal format.");
            
            unsigned long lumiEnd;
            word >> lumiEnd;
            
            
            // Save the lumi range
            lumiRanges.emplace_back(lumiStart, lumiEnd);
            
            
            c = GetNonBlank(lumiMaskFile);
            
            if (c == ']')  // List of lumi ranges ended
                break;
            else if (c != ',')
                throw runtime_error("LumiMaskFilter::LoadLumiMask: In file \""s +
                  lumiMaskFileName + "\" list of luminosity ranges for key \"" + lastKeyRead +
                  "\" is of an illegal format.");
        }
        
        
        c = GetNonBlank(lumiMaskFile);
        
        if (c == '}')  // End of JSON file
            break;
        else if (c != ',')
            throw runtime_error("LumiMaskFilter::LoadLumiMask: In file \""s + lumiMaskFileName +
              "\" syntax error after the list of lumi sections for key \"" + lastKeyRead + "\".");
    }
    
    
    // Make sure all lumi ranges are sorted. The ordering is done based on lower boundaries
    for (auto &p: lumiMask)
    {
        auto &lumiRanges = p.second;
        std::sort(lumiRanges.begin(), lumiRanges.end(),
          [](lumiRange_t const &a, lumiRange_t const &b){return (a.first < b.first);});
    }
    
    
    // Print out the mask
    #ifdef DEBUG
    cout << "Loaded luminoisty mask:\n";
    
    for (auto const &p: lumiMask)
    {
        cout << "  " << p.first << ":";
        
        for (auto const &range: p.second)
            cout << " [" << range.first << ", " << range.second << "],";
        
        cout << endl;
    }
    #endif
}


bool LumiMaskFilter::ProcessEvent()
{
     // Search for ID of the current event in the vector
    auto const &id = eventIDPlugin->GetEventID();
    bool eventFound = false;
    
    auto const runIt = lumiMask.find(id.Run());
    
    if (runIt != lumiMask.end())
    {
        auto const &lumiRanges = runIt->second;
        unsigned long const lumiNumber = id.LumiBlock();
        
        // Find first lumi range whose upper boundary is larger or equal lumi number of the current
        //event. This is thus the only lumi range that could include the current event
        auto const rangeIt = std::lower_bound(lumiRanges.begin(), lumiRanges.end(), lumiNumber,
          [](lumiRange_t const &range, unsigned long lumi){return (range.second < lumi);});
        
        // Then compare lumi number to the lower boundary of the range
        if (rangeIt != lumiRanges.end() and rangeIt->first <= lumiNumber)
            eventFound = true;
    }
    
    
    return (rejectKnownEvent) ? not eventFound : eventFound;
}


char LumiMaskFilter::GetNonBlank(std::istream &input)
{
    char symbol;
    
    do
    {
        symbol = input.get();
    }
    while ((symbol == ' ' or symbol == '\n' or symbol == '\r' or symbol == '\t') and
      not input.eof());
    
    return symbol;
}

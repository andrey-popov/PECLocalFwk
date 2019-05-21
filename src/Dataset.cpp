#include <mensura/Dataset.hpp>

#include <regex>
#include <sstream>
#include <stdexcept>


namespace fs = std::filesystem;
using namespace std;


Dataset::Dataset() noexcept:
    sourceDatasetID(""),
    isData(false),
    crossSection(0.), numEvents(0), meanWeight(0.)
{}


Dataset::Dataset(Dataset::Type type, std::string sourceDatasetID_ /*= ""*/):
    sourceDatasetID(sourceDatasetID_),
    isData(type == Type::Data),
    crossSection(0.), numEvents(0), meanWeight(0.)
{}


void Dataset::AddFile(fs::path const &path)
{
    for (auto const &p: ExpandPathMask(path))
        files.emplace_back(p);
    
    if (sourceDatasetID == "")
        SetDefaultSourceDatasetID();
}


Dataset Dataset::CopyParameters() const
{
    Dataset emptyDataset;
    emptyDataset.sourceDatasetID = sourceDatasetID;
    emptyDataset.isData = isData;
    emptyDataset.crossSection = crossSection;
    emptyDataset.numEvents = numEvents;
    emptyDataset.meanWeight = meanWeight;
    emptyDataset.flags = flags;

    return emptyDataset;
}


list<fs::path> const &Dataset::GetFiles() const
{
    return files;
}


std::string const &Dataset::GetSourceDatasetID() const
{
    return sourceDatasetID;
}


double Dataset::GetWeight() const
{
    return crossSection / (numEvents * meanWeight);
}


bool Dataset::IsMC() const
{
    return not isData;
}


void Dataset::SetFlag(string const &flagName)
{
    auto res = flags.insert(flagName);
    
    if (not res.second)  // such a flag has already been set
        throw logic_error(string("Dataset::SetFlag: Flag \"") + flagName +
         "\" has already been set.");
}


void Dataset::SetNormalization(double crossSection_, unsigned long numEvents_, double meanWeight_)
{
    crossSection = crossSection_;
    numEvents = numEvents_;
    meanWeight = meanWeight_;
}


void Dataset::UnsetFlag(string const &flagName)
{
    flags.erase(flagName);
}


bool Dataset::TestFlag(string const &flagName) const
{
    return (flags.count(flagName) > 0);
}


std::list<fs::path> Dataset::ExpandPathMask(fs::path const &path)
{
    // If the path does not include wildcards, return it as is
    if (path.string().find_first_of("*?") == std::string::npos)
    {
        // Deliberately do not attempt to check if the file actually exists
        return {path};
    }


    fs::path directoryPath{path.parent_path()};
    std::string fileNameMask{path.filename()};

    if (directoryPath.empty())
        directoryPath = ".";
    
    if (directoryPath.string().find_first_of("*?") != std::string::npos)
    {
        std::ostringstream message;
        message << "Dataset::ExpandPathMask: Directory name in path " << path <<
          " contains a wildcard, which is not supported.";
        throw std::runtime_error(message.str());
    }
    
    
    // Make sure the directory exists
    if (not fs::exists(directoryPath) or not fs::is_directory(directoryPath))
    {
        std::ostringstream message;
        message << "Dataset::ExpandPathMask: Directory \"" << directoryPath <<
          "\" does not exist or is not a valid directory.";
        throw std::runtime_error(message.str());
    }
    
    
    // Convert the file name mask into a valid regular expression. In order to do it, escape all
    //special characters except for '*' and '?' and prepend these two with dots
    std::regex escapeRegex(R"(\.|\^|\$|\||\(|\)|\[|\]|\{|\}|\+|\\)");
    fileNameMask = std::regex_replace(fileNameMask, escapeRegex, "\\$&");
    std::regex prependRegex(R"(\?|\*)");
    fileNameMask = std::regex_replace(fileNameMask, prependRegex, ".$&");
    std::regex fileNameRegex(fileNameMask);
    
    
    // Loop over the files in the directory and select ones matching the regular expression
    std::list<fs::path> concretePaths;
    
    for (fs::directory_iterator dirIt(directoryPath); dirIt != fs::directory_iterator(); ++dirIt)
    {
        if (not fs::is_regular_file(dirIt->status()))
            continue;
        
        if (std::regex_match(dirIt->path().filename().native(), fileNameRegex))
            concretePaths.emplace_back(dirIt->path().native());
    }
    
    
    // Make sure that the mask has matched at least one file
    if (concretePaths.size() == 0)
    {
        std::ostringstream message;
        message << "Dataset::ExpandPathMask: Path \"" << path << "\" does not match any file.";
        throw std::runtime_error(message.str());
    }
    
    
    return concretePaths;
}


void Dataset::SetDefaultSourceDatasetID()
{
    if (files.size() == 0)
        throw std::runtime_error("Dataset::SetDefaultSourceDatasetID: Cannot be executed when "
          "the list of files is empty.");
    
    // The source dataset ID will be based on the base name of the last added file. The base name
    //already has the extension stripped. Here strip also the optional postfix with part number.
    std::regex fileNameRegex(R"(([\._]p(art)?[0-9]+)?$)");
    sourceDatasetID = std::regex_replace(files.back().stem().string(), fileNameRegex, "");
}


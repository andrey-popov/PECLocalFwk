#include <mensura/Dataset.hpp>

#include <filesystem>
#include <regex>
#include <sstream>
#include <stdexcept>


using namespace std;


Dataset::File::File() noexcept:
    name(""), xSec(0.), nEvents(0), meanWeight(1.)
{}


Dataset::File::File(string const &name) noexcept:
    File(name, -1., 0, 1.)
{}


Dataset::File::File(string const &name_, double xSec_, unsigned long nEvents_,
  double meanWeight_ /*= 1.*/) noexcept:
    name(name_), xSec(xSec_), nEvents(nEvents_), meanWeight(meanWeight_)
{}


string Dataset::File::GetBaseName() const
{
    int const startPos = name.find_last_of('/');
    int const endPos = name.find_last_of('.');
    
    if (startPos < endPos)
        return name.substr(startPos + 1, endPos - startPos - 1);
    else
        return name.substr(startPos + 1);
}


string Dataset::File::GetDirName() const
{
    int const pos = name.find_last_of('/');
    
    if (pos >= 0)
        return name.substr(0, pos + 1);
    else
        return "./";
}


double Dataset::File::GetWeight() const
{
    return xSec / (meanWeight * nEvents);
}


Dataset::Dataset() noexcept:
    sourceDatasetID(""),
    isData(false)
{}


Dataset::Dataset(Dataset::Type type, std::string sourceDatasetID_ /*= ""*/):
    sourceDatasetID(sourceDatasetID_)
{
    if (type == Type::Data)
        isData = true;
    else if (type == Type::MC)
        isData = false;
    else
    {
        std::ostringstream message;
        message << "Dataset::Dataset: Received unknown type " << int(type) << ".";
        throw std::logic_error(message.str());
    }
}


void Dataset::AddFile(string const &path, double xSec, unsigned long nEvents,
  double meanWeight /*= 1.*/)
{
    for (auto const &p: ExpandPathMask(path))
        files.emplace_back(p, xSec, nEvents, meanWeight);
    
    if (sourceDatasetID == "")
        SetDefaultSourceDatasetID();
}


void Dataset::AddFile(string const &path)
{
    AddFile(path, -1., 0);
}


void Dataset::AddFile(Dataset::File const &file) noexcept
{
    files.push_back(file);
    
    if (sourceDatasetID == "")
        SetDefaultSourceDatasetID();
}


list<Dataset::File> const &Dataset::GetFiles() const
{
    return files;
}


std::string const &Dataset::GetSourceDatasetID() const
{
    return sourceDatasetID;
}


bool Dataset::IsMC() const
{
    return not isData;
}


Dataset Dataset::CopyParameters() const
{
    Dataset emptyDataset;
    emptyDataset.sourceDatasetID = sourceDatasetID;
    emptyDataset.isData = isData;
    emptyDataset.flags = flags;
    
    return emptyDataset;
}


void Dataset::SetFlag(string const &flagName)
{
    auto res = flags.insert(flagName);
    
    if (not res.second)  // such a flag has already been set
        throw logic_error(string("Dataset::SetFlag: Flag \"") + flagName +
         "\" has already been set.");
}


void Dataset::UnsetFlag(string const &flagName)
{
    flags.erase(flagName);
}


bool Dataset::TestFlag(string const &flagName) const
{
    return (flags.count(flagName) > 0);
}


std::list<std::string> Dataset::ExpandPathMask(std::string const &path)
{
    namespace fs = std::filesystem;
    
    // Check if the path includes a wildcard
    auto const wildcardPos = path.find_first_of("*?");
    
    if (wildcardPos == std::string::npos)
    {
        // Deliberately do not attempt to check if the file actually exists
        return {path};
    }
    
    
    // Split the path into directory path and file mask and make sure that the directory path does
    //not contain wildcards
    auto const lastSlashPos = path.find_last_of('/');
    
    if (lastSlashPos != std::string::npos and wildcardPos < lastSlashPos)
    {
        std::ostringstream message;
        message << "Dataset::ExpandPathMask: Directory name in path \"" << path <<
          "\" contains a wildcard, which is not supported.";
        throw std::runtime_error(message.str());
    }
    
    std::string directoryPath, fileNameMask;
    
    if (lastSlashPos != std::string::npos)
    {
        directoryPath = path.substr(0, lastSlashPos);
        fileNameMask = path.substr(lastSlashPos + 1);
    }
    else
    {
        directoryPath = ".";
        fileNameMask = path;
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
    std::list<std::string> concretePaths;
    
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
    sourceDatasetID = std::regex_replace(files.back().GetBaseName(), fileNameRegex, "");
}


#include <mensura/core/Dataset.hpp>

#include <boost/filesystem.hpp>

#include <regex>
#include <sstream>
#include <stdexcept>


using namespace std;


Dataset::File::File() noexcept:
    name(""), xSec(0.), nEvents(0)
{}


Dataset::File::File(string const &name) noexcept:
    File(name, -1., 0)
{}


Dataset::File::File(string const &name_, double xSec_, unsigned long nEvents_) noexcept:
    name(name_), xSec(xSec_), nEvents(nEvents_)
{}


string Dataset::File::GetBaseName() const noexcept
{
    int const startPos = name.find_last_of('/');
    int const endPos = name.find_last_of('.');
    
    if (startPos < endPos)
        return name.substr(startPos + 1, endPos - startPos - 1);
    else
        return name.substr(startPos + 1);
}


string Dataset::File::GetDirName() const noexcept
{
    int const pos = name.find_last_of('/');
    
    if (pos >= 0)
        return name.substr(0, pos + 1);
    else
        return "./";
}


Dataset::Dataset() noexcept:
    sourceDatasetID(""),
    isData(false),
    processCodes({Process::Undefined}),
    generator(Generator::Undefined),
    showerGenerator(ShowerGenerator::Undefined)
{}


Dataset::Dataset(Dataset::Type type, std::string sourceDatasetID_ /*= ""*/):
    sourceDatasetID(sourceDatasetID_),
    processCodes({Process::Undefined}),
    generator(Generator::Undefined),
    showerGenerator(ShowerGenerator::Undefined)
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


Dataset::Dataset(list<Dataset::Process> &&processCodes_,
 Dataset::Generator generator_ /*= Dataset::Generator::Undefined*/,
 Dataset::ShowerGenerator showerGenerator_ /*= Dataset::ShowerGenerator::Undefined*/) noexcept:
    sourceDatasetID(""),
    processCodes(SortProcessCodes(move(processCodes_))),
    generator(generator_),
    showerGenerator(showerGenerator_)
{
    Init();
}


Dataset::Dataset(list<Dataset::Process> const &processCodes_,
 Dataset::Generator generator_ /*= Dataset::Generator::Undefined*/,
 Dataset::ShowerGenerator showerGenerator_ /*= Dataset::ShowerGenerator::Undefined*/) noexcept:
    sourceDatasetID(""),
    processCodes(SortProcessCodes(processCodes_)),
    generator(generator_),
    showerGenerator(showerGenerator_)
{
    Init();
}


Dataset::Dataset(Dataset::Process process,
 Dataset::Generator generator_ /*= Dataset::Generator::Undefined*/,
 Dataset::ShowerGenerator showerGenerator_ /*= Dataset::ShowerGenerator::Undefined*/) noexcept:
    sourceDatasetID(""),
    processCodes({process}),
    generator(generator_),
    showerGenerator(showerGenerator_)
{
    Init();
}


void Dataset::AddFile(string const &path, double xSec, unsigned long nEvents)
{
    for (auto const &p: ExpandPathMask(path))
        files.emplace_back(p, xSec, nEvents);
    
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


Dataset::Generator Dataset::GetGenerator() const
{
    return generator;
}


Dataset::ShowerGenerator Dataset::GetShowerGenerator() const
{
    return showerGenerator;
}


Dataset::Process Dataset::GetProcess() const
{
    return processCodes.back();
}


list<Dataset::Process> const &Dataset::GetProcessCodes() const
{
    return processCodes;
}


bool Dataset::TestProcess(Dataset::Process code) const
{
    for (Process const &c: processCodes)
        if (c == code)
            return true;
    
    // If the workflow reaches this point, the required process code has not been found
    return false;
}


bool Dataset::IsMC() const
{
    return not isData;
}


Dataset Dataset::CopyParameters() const
{
    Dataset emptyDataset(processCodes, generator, showerGenerator);
    emptyDataset.flags = flags;
    // The sourceDatasetID is intentionally not copied. It will be set when files are added to
    //the newly created dataset
    
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
    namespace fs = boost::filesystem;
    
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


list<Dataset::Process> Dataset::SortProcessCodes(list<Dataset::Process> &&processCodes)
{
    processCodes.sort([](Process code1, Process code2){return (int(code1) < int(code2));});
    
    return processCodes;
}


list<Dataset::Process> Dataset::SortProcessCodes(list<Dataset::Process> const &origProcessCodes)
{
    auto processCodes = origProcessCodes;
    SortProcessCodes(move(processCodes));
    
    return processCodes;
}


void Dataset::Init()
{
    Process const process = processCodes.front();
    
    if (process == Process::ppData or process == Process::pp7TeV or process == Process::pp8TeV or
     process == Process::pp13TeV)
    //^ Codes pp*TeV are checked to recover from a potential user's error when (s)he does not set
    //ppData. It also ensures backward compatibility
    {
        isData = true;
        
        if (generator == Generator::Undefined)
            generator = Generator::Nature;
        
        if (showerGenerator == ShowerGenerator::Undefined)
            showerGenerator = ShowerGenerator::Nature;
    }
    else
        isData = false;
}

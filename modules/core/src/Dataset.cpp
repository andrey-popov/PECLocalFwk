#include <mensura/core/Dataset.hpp>

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
    processCodes({Process::Undefined}),
    generator(Generator::Undefined),
    showerGenerator(ShowerGenerator::Undefined)
{}


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


void Dataset::AddFile(string const &name, double xSec, unsigned long nEvents) noexcept
{
    files.emplace_back(name, xSec, nEvents);
    
    if (sourceDatasetID == "")
        SetDefaultSourceDatasetID();
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
    switch (processCodes.front())
    {
        case Process::ppData:
        case Process::pp7TeV:
        case Process::pp8TeV:
        case Process::pp13TeV:
        //^ User is expected to set ppData for any dataset with real data, but pp*TeV are checked as
        //well to tolerate missing ppData. This check also ensures backward compatibility
            return false;
        
        default:
        //^ Note that Undefined goes here
            return true;
    }
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


void Dataset::SetDefaultSourceDatasetID()
{
    if (files.size() == 0)
        throw std::runtime_error("Dataset::SetDefaultSourceDatasetID: Cannot be executed when "
          "the list of files is empty.");
    
    
    // Set the ID label to the short name of the last input file and strip the part number postfix
    //from it if present
    sourceDatasetID = files.back().GetBaseName();
    unsigned lastChar = sourceDatasetID.size() - 1;
    
    if (sourceDatasetID[lastChar] >= '0' and sourceDatasetID[lastChar] <= '9')
    {
        --lastChar;
        
        while (sourceDatasetID[lastChar] >= '0' and sourceDatasetID[lastChar] <= '9')
         --lastChar;
        
        if (sourceDatasetID[lastChar] == 'p' and sourceDatasetID[lastChar - 1] == '_')
        {
            // There is a postfix with the part number. Update the ID label stripping the postfix
            sourceDatasetID = sourceDatasetID.substr(0, lastChar - 1);
        }
    }
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
        if (generator == Generator::Undefined)
            generator = Generator::Nature;
        
        if (showerGenerator == ShowerGenerator::Undefined)
            showerGenerator = ShowerGenerator::Nature;
    }
}

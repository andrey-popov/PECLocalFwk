#include <Dataset.hpp>

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
    processCodes({Dataset::Process::Undefined}),
    generator(Dataset::Generator::Undefined),
    showerGenerator(Dataset::ShowerGenerator::Undefined)
{}


Dataset::Dataset(list<Dataset::Process> const &processCodes_,
 Dataset::Generator generator_ /*= Dataset::Generator::Undefined*/,
 Dataset::ShowerGenerator showerGenerator_ /*= Dataset::ShowerGenerator::Undefined*/) noexcept:
    processCodes(processCodes_),
    generator(generator_),
    showerGenerator(showerGenerator_)
{
    Process const process = processCodes.front();
    
    if (process == Process::pp7TeV or process == Process::pp8TeV)
    {
        if (generator == Generator::Undefined)
            generator = Generator::Nature;
        
        if (showerGenerator == ShowerGenerator::Undefined)
            showerGenerator = ShowerGenerator::Nature;
    }
}


Dataset::Dataset(Dataset::Process process,
 Dataset::Generator generator /*= Dataset::Generator::Undefined*/,
 Dataset::ShowerGenerator showerGenerator /*= Dataset::ShowerGenerator::Undefined*/) noexcept:
    // Construction is delegated to the more general constructor
    Dataset({process}, generator, showerGenerator)
{}


void Dataset::AddFile(string const &name, double xSec, unsigned long nEvents) noexcept
{
    files.emplace_back(name, xSec, nEvents);
}


void Dataset::AddFile(Dataset::File const &file) noexcept
{
    files.push_back(file);
}


list<Dataset::File> const &Dataset::GetFiles() const
{
    return files;
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


bool Dataset::IsMC() const
{
    return (generator != Generator::Nature and generator != Generator::Undefined);
}


Dataset Dataset::CopyParameters() const
{
    Dataset emptyDataset(processCodes, generator, showerGenerator);
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
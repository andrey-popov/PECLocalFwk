#include <mensura/TFileService.hpp>

#include <mensura/Dataset.hpp>

#include <boost/algorithm/string/predicate.hpp>
#include <boost/filesystem.hpp>

#include <algorithm>


using namespace std::literals::string_literals;


TFileService::TFileService(std::string const &name, std::string const &outFileName_):
    Service(name),
    outFileName(outFileName_)
{
    CheckOutputPath();
}


TFileService::TFileService(std::string const &outFileName_):
    Service("TFileService"),
    outFileName(outFileName_)
{
    CheckOutputPath();
}


TFileService::TFileService(TFileService const &src) noexcept:
    Service(src),
    outFileName(src.outFileName)
{}


TFileService::~TFileService() noexcept
{}


void TFileService::BeginRun(Dataset const &dataset)
{
    // Build actual name of the output file by replacing the substitute symbol with the name of
    //the first file in the dataset
    std::string curOutFileName(outFileName);
    auto const substitutionPos = curOutFileName.find("%");
    
    if (substitutionPos != std::string::npos)
        curOutFileName.replace(substitutionPos, 1, dataset.GetFiles().front().GetBaseName());
    
    
    // Create the output file
    ROOTLock::Lock();
    outFile.reset(new TFile(curOutFileName.c_str(), "recreate"));
    ROOTLock::Unlock();
}


Service *TFileService::Clone() const
{
    return new TFileService(*this);
}


void TFileService::EndRun()
{
    // Write all objects associated with the current file and close it
    ROOTLock::Lock();
    outFile->Write();
    outFile.reset();
    ROOTLock::Unlock();
}


void TFileService::CheckOutputPath()
{
    // Split output path into directory and file name
    boost::filesystem::path const outputPath(outFileName);
    std::string const directoryName(outputPath.parent_path().string());
    std::string const fileName(outputPath.filename().string());
    
    
    // Make sure the file name is a valid
    if (fileName == "" or fileName == "." or fileName == ".." or fileName == "/")
        throw std::runtime_error("TFileService::CheckOutputPath: Path \""s + outFileName +
          "\" does not include a valid file name.");
    
    
    // Verify that there is at most one substitution symbol, and it is not included in the
    //directory name
    if (directoryName.find("%") != std::string::npos)
        throw std::runtime_error("TFileService::CheckOutputPath: Path \""s + outFileName +
          "\" includes a substitution in the directory path, which is not supported.");
    
    if (std::count(fileName.begin(), fileName.end(), '%') > 1)
        throw std::runtime_error("TFileService::CheckOutputPath: Path \""s + outFileName +
          "\" includes more than one substitution, which is not supported.");
    
    
    // Create the directories
    if (directoryName != "")
        boost::filesystem::create_directories(directoryName);
    
    
    // Add file extension if needed
    if (not boost::algorithm::ends_with(outFileName, ".root"))
        outFileName += ".root";
}

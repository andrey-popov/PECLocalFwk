#pragma once

#include <mensura/core/Service.hpp>

#include <mensura/core/ROOTLock.hpp>

#include <TFile.h>

#include <memory>
#include <stdexcept>
#include <string>


/**
 * \class TFileService
 * \brief Creates ROOT files and allows writing output into them
 * 
 * This service opens for writing a ROOT file for each processed dataset and allows creation of
 * ROOT objects, such as histograms and trees, to be stored in the file. The main motivation behind
 * this service is to allow aggregating output of multiple plugins in a single output file.
 */
class TFileService: public Service
{
public:
    /**
     * \brief Creates a service with the given name and given path for the output file
     * 
     * The given path may include directories, which will be created if needed. The filename part
     * of the path may contain one symbol "%", which will be replaced by the base name of the
     * first file in each dataset. The extension ".root" is optional and is added automatically if
     * omitted.
     */
    TFileService(std::string const &name, std::string const &outFileName);
    
    /// A short-cut for the above version with a default name "TFileService"
    TFileService(std::string const &outFileName = "%");
    
    /// Default move constructor
    TFileService(TFileService &&) = default;
    
    /// Assignment operator is deleted
    TFileService &operator=(TFileService const &) = delete;
    
    /// Trivial destructor
    virtual ~TFileService() noexcept;
    
private:
    /**
     * \brief Copy constructor
     * 
     * A valid copy can only be constructed before the first call to BeginRun. For the sake of
     * protection, this constructor cannot be used directly.
     */
    TFileService(TFileService const &src) noexcept;
    
public:
    /**
     * \brief Creates an output file for the new dataset
     * 
     * Reimplemented from Service.
     */
    virtual void BeginRun(Dataset const &dataset) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Service.
     */
    virtual Service *Clone() const override;
    
    /**
     * \brief Creates a ROOT object in the given directory in the output file
     * 
     * The directory path may include subdirectories. Directories are created if they do not exist.
     * In order to create an object in the root directory, use "". Arguments 'args' are forwarded
     * to the constructor of T. The created object is owned by the output file.
     */
    template <typename T, typename ... Args>
    T *Create(std::string const &inFileDirectory, Args const &... args) const;
    
    /**
     * \brief Writes and closes the output file
     * 
     * Reimplemented from Service.
     */
    virtual void EndRun() override;
    
private:
    /// Validates provided output path and creates directories if needed
    void CheckOutputPath();
    
private:
    /// Pattern with the name of the output file
    std::string outFileName;
    
    /// Output file for the current dataset
    std::unique_ptr<TFile> outFile;
};


template <typename T, typename ... Args>
T *TFileService::Create(std::string const &inFileDirectory, Args const &... args) const
{
    // Make sure the output file exists
    if (not outFile)
        throw std::runtime_error("TFileService::Create: This method is called before the output "
          "file is created.");
    
    
    ROOTLock::Lock();
    
    // Change to the given directory. Create it if needed
    TDirectory *d = outFile->GetDirectory(inFileDirectory.c_str());
    
    if (not d)
        d = outFile->mkdir(inFileDirectory.c_str());
    
    d->cd();
    
    
    // Create the ROOT object in the new current directory
    T *object = new T(args...);
    
    ROOTLock::Unlock();
    
    
    return object;
}

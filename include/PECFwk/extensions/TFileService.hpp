#pragma once

#include <PECFwk/core/Service.hpp>

#include <TFile.h>

#include <memory>
#include <string>


/**
 * \class TFileService
 * \brief Creates ROOT files and allows writing output into them
 * 
 * 
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
    
    /// A short-cut for the above version with a default name "BTagEff"
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
     * \brief Writes and closes the output file
     * 
     * Reimplemented from Service.
     */
    virtual void EndRun() override;
    
    /// Returns non-owning pointer to the current output file
    TFile *GetFile() const;
    
private:
    /// Validates provided output path and creates directories if needed
    void CheckOutputPath();
    
private:
    /// Pattern with the name of the output file
    std::string outFileName;
    
    /// Output file for the current dataset
    std::unique_ptr<TFile> outFile;
};

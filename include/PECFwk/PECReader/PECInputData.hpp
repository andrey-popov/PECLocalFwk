#pragma once

#include <PECFwk/core/ReaderPlugin.hpp>

#include <PECFwk/core/Dataset.hpp>
#include <PECFwk/PECReader/EventID.hpp>

#include <TFile.h>
#include <TTree.h>

#include <memory>
#include <string>

/**
 * \class PECInputData
 * \brief Opens and owns an input file in PEC format
 * 
 * 
 */
class PECInputData: public ReaderPlugin
{
public:
    // PECInputData() = delete;
    
    PECInputData(std::string const name = "");
    
    /// The copy constructor is deleted
    PECInputData(PECInputData const &) = delete;
    
    /// Default move constructor
    PECInputData(PECInputData &&) = default;
    
    /// Assignment operator is deleted
    PECInputData &operator=(PECInputData const &) = delete;
    
    /// Trivial destructor
    virtual ~PECInputData();
    
public:
    virtual Plugin *Clone() const override;
    
    virtual void BeginRun(Dataset const &dataset) override;
    
    virtual void EndRun() override;
    
    TTree *GetTree(std::string const &name);
    
private:
    bool NextInputFile();
    virtual bool ProcessEvent() override;
    
private:
    /// Files in the current dataset
    std::list<Dataset::File> inputFiles;
    
    /// Iterator pointing to the next file in the dataset
    std::list<Dataset::File>::const_iterator nextFileIt;
    
    /// Currently opened input file
    std::unique_ptr<TFile> curInputFile;
    
    /// Tree with event IDs from the current input file
    std::unique_ptr<TTree> eventIDTree;
    
    /// Total number of events in the current tree
    unsigned long nEvents;
    
    /// Index of the next event to read from the tree
    unsigned long nextEvent;
    
    /// Buffer to read event ID branch of the tree
    pec::EventID bfEventID;
    
    /**
     * \brief An auxiliary pointer
     * 
     * ROOT needs a variable with a pointer to an object to read the object from a tree.
     */
    pec::EventID *bfEventIDPointer;
};

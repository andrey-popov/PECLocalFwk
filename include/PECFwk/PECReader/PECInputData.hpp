#pragma once

#include <PECFwk/core/ReaderPlugin.hpp>

#include <PECFwk/core/EventID.hpp>
#include <PECFwk/core/Dataset.hpp>
#include <PECFwk/PECReader/EventID.hpp>

#include <TFile.h>
#include <TTree.h>

#include <memory>
#include <string>


/**
 * \class PECInputData
 * \brief Opens files in PEC format
 * 
 * The plugin opens files in a dataset in PEC format. It only reads the tree with event ID,
 * exploiting it also to judge when there are no more events in an input file. Other plugins can
 * request it to extract trees from the file.
 * 
 * Currently the framework does not provide an elegant way to notify other plugins when a new file
 * is opened. This is a problem because dependant plugins much update their pointers to trees and
 * set up branch addresses whenever a new input file is opened. However, the framework typically
 * operates with atomic datasets, so this should not be a large problem. For the time being,
 * reading of multiple files in a dataset is disabled; only the first file will be read.
 */
class PECInputData: public ReaderPlugin
{
public:
    /// Creates a new plugin with the given name
    PECInputData(std::string const name = "InputData");
    
    /// The copy constructor is deleted
    PECInputData(PECInputData const &) = delete;
    
    /// Default move constructor
    PECInputData(PECInputData &&) = default;
    
    /// Assignment operator is deleted
    PECInputData &operator=(PECInputData const &) = delete;
    
    /// Trivial destructor
    virtual ~PECInputData();
    
public:
    /**
     * \brief Performs initialization for a new dataset
     * 
     * Reimplemented from Plugin.
     */
    virtual void BeginRun(Dataset const &dataset) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Plugin.
     */
    virtual Plugin *Clone() const override;
    
    /// Returns ID of the current event
    EventID const &GetEventID() const;
    
    /**
     * \brief Reads the tree with the given name from the current file
     * 
     * Since event ID is read by this class, the method does not allow to read the corresponding
     * tree again and will throw an exception is an attempt of this. This safety check is not
     * in place yet, but in future the method could keep a list of requested trees and flag report
     * an error if one of the trees is requested for the second time.
     */
    std::unique_ptr<TTree> GetTree(std::string const &name) const;
    
private:
    /**
     * \brief Opens the next input file in the dataset
     * 
     * Returns true in case of success and false if there are no more input files left.
     */
    bool NextInputFile();
    
    /**
     * \brief Reads the next event
     * 
     * Reads ID of the next event in the current input file. Calls NextInputFile if there are no
     * events left in the file. Returns true in case of success and false if there are no more
     * events in the dataset.
     * Implemented from Plugin.
     */
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
    
    /// ID of the current event converted into the standard format of the framework
    EventID eventID;
};

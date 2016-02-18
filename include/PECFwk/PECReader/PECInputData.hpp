#pragma once

#include <PECFwk/core/ReaderPlugin.hpp>

#include <PECFwk/core/EventID.hpp>
#include <PECFwk/core/Dataset.hpp>
#include <PECFwk/PECReader/EventID.hpp>

#include <TFile.h>
#include <TTree.h>

#include <map>
#include <memory>
#include <string>


/**
 * \class PECInputData
 * \brief Opens files in PEC format and provides access to stored trees
 * 
 * The plugin opens files in a dataset in PEC format. It reads the tree with event ID, exploiting
 * it also to judge when there are no more events in an input file. Other plugins can request it
 * to extract additional trees from the file. The trees are owned by this plugin.
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
    /// Status codes for method LoadTree
    enum class LoadTreeStatus
    {
        Success,  ///< Everything is fine
        AlreadyLoaded,  ///< Requested tree has already been loaded by another plugin
        NotFound  ///< Requested tree is not found in the input file
    };
    
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
    
    /**
     * \brief Returns non-owning pointer to the loaded tree with the given name
     * 
     * The must be loaded with method LoadTree beforehand. If it is not the case, an exception is
     * thrown. The user should not perform destructive operations using the returned pointer. Note
     * that it is not possible to call TTree::GetEntry with this pointer since the index of the
     * current event is not exposed. To read the current event use the ReadEventFromTree method.
     */
    TTree *ExposeTree(std::string const &name) const;
    
    /// Returns ID of the current event
    EventID const &GetEventID() const;
    
    /**
     * \brief Reads the tree with the given name from the current file
     * 
     * If the tree has already been loaded, prints an error and returns an appropriate status code.
     * If the tree does not exist in the input file, it is not added to the collection of loaded
     * trees and an appropriate status code is returned. If the tree is read from the file
     * successfully but contains a different number of entries than the event ID tree, an exception
     * is thrown.
     */
    LoadTreeStatus LoadTree(std::string const &name);
    
    /**
     * \brief Reads current event in the tree with the given name
     * 
     * Throws an exception if the tree has not been loaded.
     */
    void ReadEventFromTree(std::string const &name) const;
    
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
    
    /// Name of the tree with event IDs
    std::string eventIDTreeName;
    
    /// Total number of events in the current tree
    unsigned long nEvents;
    
    /// Index of the next event to read from the tree
    unsigned long nextEvent;
    
    /**
     * \brief Map of loaded trees
     * 
     * The map key is the tree name. Since the number of trees in a PEC file is small, a map should
     * perform better than an unordered_map.
     */
    std::map<std::string, std::unique_ptr<TTree>> loadedTrees;
    
    /// A shortcut to the tree with event IDs
    TTree *eventIDTree;
    
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

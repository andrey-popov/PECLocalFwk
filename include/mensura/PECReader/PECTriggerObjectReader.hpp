#pragma once

#include <mensura/PhysicsObjects.hpp>
#include <mensura/ReaderPlugin.hpp>

#include <mensura/PECReader/Candidate.hpp>

#include <initializer_list>
#include <map>
#include <regex>
#include <string>
#include <vector>


class PECInputData;


/**
 * \class PECTriggerObjectReader
 * \brief Reads trigger objects for selected filters
 * 
 * This plugin reads trigger objects that have been accepted by trigger filters. It is possible to
 * select which of filters stored in a PEC file to read. The objects are represented by class
 * Candidate.
 */
class PECTriggerObjectReader: public ReaderPlugin
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * The plugin will only read trigger objects for filters whose names match at least one of the
     * given mask. The masks are interpreted as regular expressions. By default a mask that matches
     * any name is used.
     */
    PECTriggerObjectReader(std::string const &name,
      std::initializer_list<std::string> triggerMasks = {".*"});
    
    /**
     * \brief Crates plugin with default name "TriggerObjects"
     * 
     * See documentation for the first version of constructor for details.
     */
    PECTriggerObjectReader(std::initializer_list<std::string> triggerMasks = {".*"});
    
    /// Default copy constructor
    PECTriggerObjectReader(PECTriggerObjectReader const &src) = default;
    
    /// Default move constructor
    PECTriggerObjectReader(PECTriggerObjectReader &&) = default;
    
    /// Assignment operator is deleted
    PECTriggerObjectReader &operator=(PECTriggerObjectReader const &) = delete;
    
public:
    /**
     * \brief Sets up reading of a tree with trigger objects
     * 
     * If the tree contains no branches that match at least one of the masks given to constructor,
     * an exception is thrown.
     * 
     * Reimplemented from Plugin.
     */
    virtual void BeginRun(Dataset const &) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Plugin.
     */
    virtual Plugin *Clone() const override;
    
    /**
     * \brief Returns index of the requested trigger filter for a fast access
     * 
     * Throws an exception if the filter is not known (because its name does not satisfy any of the
     * masks given to constructor or because the input tree does not contain such branch).
     */
    unsigned GetFilterIndex(std::string const &triggerFilterName) const;
    
    /**
     * \brief Returns collection of trigger objects for the given trigger filter
     * 
     * If the filter is not known (because its name does not satisfy any of the masks given to
     * constructor or because the input tree does not contain such branch), an exception is thrown.
     */
    std::vector<Candidate> const &GetObjects(std::string const &triggerFilterName) const;
    
    /**
     * \brief Returns collection of trigger objects for the filter with the given index
     * 
     * This is a bit faster version of the above method. Throws an exception if the given index is
     * out of range.
     */
    std::vector<Candidate> const &GetObjects(unsigned filterIndex) const;
    
private:
    /**
     * \brief Reads trigger objects for selected filters from input tree
     * 
     * Reimplemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Name of the plugin that reads PEC files
    std::string inputDataPluginName;
    
    /// Non-owning pointer to a plugin that reads PEC files
    PECInputData const *inputDataPlugin;
    
    /// Name of the tree containing information about trigger objects
    std::string treeName;
    
    /// Masks to choose trigger filters to read
    std::vector<std::regex> triggerMasks;
    
    /// Mapping from trigger filter names to buffer indices
    std::map<std::string, unsigned> triggerIndexMap;
    
    /**
     * \brief Vector of buffers to read collections of trigger objects
     * 
     * Indices of the outer vector are stored in triggerIndexMap.
     */
    std::vector<std::vector<pec::Candidate>> buffers;
    
    /**
     * \brief Auxiliary pointers to buffers
     * 
     * Need by ROOT to read the objects from a tree. Indices of the outer vector are stored in
     * triggerIndexMap.
     */
    std::vector<std::vector<pec::Candidate> *> bufferPointers;
    
    /**
     * \brief Collections of trigger objects translated into standard classes
     * 
     * Indices of the outer vector are stored in triggerIndexMap.
     */
    std::vector<std::vector<Candidate>> triggerObjects;
};

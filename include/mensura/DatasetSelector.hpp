#pragma once

#include <mensura/AnalysisPlugin.hpp>

#include <initializer_list>
#include <regex>
#include <string>
#include <vector>


/**
 * \class DatasetSelector
 * \brief Implements selection based on dataset ID
 * 
 * The source dataset ID is checked against regex masks provided by the user. If a match is found
 * for at least one of the masks, all events in the current dataset are accepted, and vice versa.
 * This logic can be inverted with a dedicated flag in the configuration. In that case all events
 * in a dataset will be rejected if its source dataset ID matches at least one of the provided
 * masks.
 */
class DatasetSelector: public AnalysisPlugin
{
public:
    /**
     * \brief Constructs a selector with a given name
     * 
     * The second argument contains a collection of regex masks to select datasets. The source
     * dataset ID needs to match at least one of the mask for the dataset to be accepted. The logic
     * can be inverted using the flag inverse; in that case the dataset will be rejected if its ID
     * matches at least one of the provided masks.
     */
    DatasetSelector(std::string const &name, std::initializer_list<std::string> const &masks,
      bool inverse = false);
    
    /// A short-cut for the above version with a default name "DatasetSelector"
    DatasetSelector(std::initializer_list<std::string> const &masks, bool inverse = false);
    
    /// Default copy constructor
    DatasetSelector(DatasetSelector const &) = default;
    
public:
    /**
     * \brief Checks if the current dataset is to be processed
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
    
private:
    /**
     * \brief Keeps or rejects the current event depending on whether the dataset is selected.
     * 
     * Reimplemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Masks to choose in which datasets to compute weights
    std::vector<std::regex> datasetMasks;
    
    /// Flag indicating inversed logic
    bool inversedLogic;
    
    /// Decision for the current dataset
    bool processCurDataset;
};

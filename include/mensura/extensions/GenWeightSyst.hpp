#pragma once

#include <mensura/extensions/EventWeightPlugin.hpp>

#include <initializer_list>
#include <string>
#include <vector>
#include <utility>


class GeneratorReader;


/**
 * \class GenWeightSyst
 * \brief A plugin to compute systematic variation due to generator-level weights
 * 
 * 
 */
class GenWeightSyst: public EventWeightPlugin
{
public:
    /**
     * \brief Constructs a new reweighting plugin with the given name
     * 
     * The second argument is a vector of indices of weights for systematic variations. The indices
     * will be used to access the weights via GeneratorReader::GetAltWeight. Each pair corresponds
     * to an independent systematic variation, first weight is for "up" variation, second is for
     * "down" one.
     */
    GenWeightSyst(std::string const name,
      std::initializer_list<std::pair<unsigned, unsigned>> const &systWeightsIndices);
    
    /// A short-cut for the above version with a default name "GenWeightSyst"
    GenWeightSyst(std::initializer_list<std::pair<unsigned, unsigned>> const &systWeightsIndices);
    
    /// Copy constructor
    GenWeightSyst(GenWeightSyst const &src);
    
    /// Default move constructor
    GenWeightSyst(GenWeightSyst &&) = default;
    
    /// Assignment operator is deleted
    GenWeightSyst &operator=(GenWeightSyst const &) = delete;
    
public:
    /**
     * \brief Saves pointers to dependencies
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
    
private:
    /**
     * \brief Computes event weights
     * 
     * Reimplemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Name of plugin that provides generator-level weights
    std::string generatorReaderName;
    
    /// Non-owning pointer to plugin that provides generator-level weights
    GeneratorReader const *generatorReader;
    
    /// Index of the reference weight used for per-event normalization
    unsigned referenceWeightIndex;
    
    /**
     * \brief Indices of weights for systematic variations
     * 
     * In each pair first weight corresponds to "up" variation and second one to the "down"
     * variation.
     */
    std::vector<std::pair<unsigned, unsigned>> systWeightsIndices;
};

#pragma once

#include <mensura/EventWeightPlugin.hpp>

#include <initializer_list>
#include <map>
#include <string>
#include <vector>
#include <utility>


class GeneratorReader;


/**
 * \class GenWeightSyst
 * \brief A plugin to compute systematic variation due to generator-level weights
 * 
 * This reweighting plugin reads generator-level weights with specified indices and computes
 * systematic variations based on them. Nominal weight is always unit. Weights are accessed from a
 * GeneratorReader with a default name "Generator".
 * 
 * Indices of weights are organized in pairs corresponding to "up" and "down" variations. Depending
 * on what constructor is used, they can be specified in a JSON file, individually for each
 * dataset, or the same set of weight pairs can be used for all datasets.
 * 
 * There are three modes of running. In the simplest case weights with specified indices are used
 * directly, after they are divided by a reference weight in the current event (by default given by
 * index 0). Using method NormalizeByMeanWeights, user can request that weights are normalized by
 * their mean values read from a database file. In this case a weight w_i is rescaled as
 *   (w_i / w_0) / (<w_i> / <w_0>),
 * where w_0 is the reference weight, and <w> denotes corresponding mean weights. Rescaling by
 * mean weights allows to preserve the total normalization. Finally, if the rescaling has been
 * requested but the database file does not contain entry for the current dataset, systematic
 * variations are not evaluated and their number is set to zero.
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
    GenWeightSyst(std::string const &name,
      std::initializer_list<std::pair<unsigned, unsigned>> const &systWeightsIndices);
    
    /// A short-cut for the above version with a default name "GenWeightSyst"
    GenWeightSyst(std::initializer_list<std::pair<unsigned, unsigned>> const &systWeightsIndices);
    
    /**
     * \brief Constructs a new reweighting plugin with dataset-specific weight indices
     * 
     * The pairs of weight indices are read from a JSON file, which provides an independent list
     * for each dataset ID. The syntax is as in the following example:
     *   [
     *     {
     *       "datasetId": "ttbar-pw_320_hFE",
     *       "weightPairs": [[1, 2], [3, 6], [4, 8], [240, 231]]
     *     },
     *     ...
     *   ]
     * A default set of index pairs can be provided as an entry with datasetId "*".
     */
    GenWeightSyst(std::string const &name, std::string const &weightIndicesFile);
    
    /// A short-cut for the above version with a default name "GenWeightSyst"
    GenWeightSyst(std::string const &weightIndicesFile);
    
    /// Copy constructor
    GenWeightSyst(GenWeightSyst const &src);
    
    /// Default move constructor
    GenWeightSyst(GenWeightSyst &&) = default;
    
    /// Assignment operator is deleted
    GenWeightSyst &operator=(GenWeightSyst const &) = delete;
    
public:
    /**
     * \brief Saves pointers to dependencies and find mean weights for the current dataset
     * 
     * The method also verifies that mean weights for all requested indices are avaiable for the
     * current dataset and throws an except otherwise.
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
     * \brief Requests that weight are normalized by their mean values
     * 
     * The values are read from the given JSON file, whose location is resolved using FileInPath.
     * Example structure of the file is shown below:
     * 
     *  [
     *    {
     *      "datasetId": "ttbar-pw_3.1.0_wdo",
     *      "meanLHEWeights": [
     *        {
     *          "index": 0,
     *          "value": 1.0
     *        },
     *        {
     *          "index": 4,
     *          "value": 0.8726374369
     *        }
     *      ]
     *    },
     *    ...
     *  ]
     * 
     * Here field "datasetId" includes label uniquely identifying the sample as returned by
     * Dataset::GetSourceDatasetID, and indices correspond to GeneratorReader::GetAltWeight.
     */
    void NormalizeByMeanWeights(std::string const &databaseFile);
    
private:
    /**
     * \brief Returns a vector of pairs of weight indices for the given dataset ID.
     * 
     * The returned pointer refers to an element in map systWeightsIndices. It might be null if
     * the map does not contain the given dataset ID, and there is no default.
     */
    std::vector<std::pair<unsigned, unsigned>> const *FindWeightIndices(
      std::string const &datasetID) const;
    
    /**
     * \brief Computes systematic weights
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
     * Key of the map is the dataset ID. In each pair first weight corresponds to "up" variation
     * and second one to the "down" variation.
     */
    std::map<std::string, std::vector<std::pair<unsigned, unsigned>>> systWeightsIndices;
    
    /// Indices of weights for systematic variations for the current dataset
    std::vector<std::pair<unsigned, unsigned>> const *systWeightsIndicesCurDataset;
    
    /// Flag showing if weights should be rescaled by their mean values
    bool rescaleWeights;
    
    /**
     * \brief Mean values of generator-level weights to be used in renormalization
     * 
     * Key of the first map is dataset ID, key of the second one is weight index.
     */
    std::map<std::string, std::map<unsigned, double>> meanWeights;
    
    /**
     * \brief Mean values of generator-level weights for the current dataset
     * 
     * This is a pointer to an element of map meanWeights. It is null if mean weights for the
     * current dataset have not been found or if weights do not need to be rescaled.
     */
    std::map<unsigned, double> const *meanWeightsCurDataset;
};

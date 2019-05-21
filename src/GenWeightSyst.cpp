#include <mensura/GenWeightSyst.hpp>

#include <mensura/Config.hpp>
#include <mensura/Dataset.hpp>
#include <mensura/GeneratorReader.hpp>
#include <mensura/Logger.hpp>

#include <mensura/external/JsonCpp/json.hpp>

#include <algorithm>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>


GenWeightSyst::GenWeightSyst(std::string const &name,
  std::initializer_list<std::pair<unsigned, unsigned>> const &defaultSystWeightsIndices):
    EventWeightPlugin(name),
    generatorReaderName("Generator"), generatorReader(nullptr),
    referenceWeightIndex(0),
    systWeightsIndicesCurDataset(nullptr),
    rescaleWeights(false),
    meanWeightsCurDataset(nullptr)
{
    systWeightsIndices.emplace(std::piecewise_construct, std::forward_as_tuple("*"),
      std::forward_as_tuple(defaultSystWeightsIndices));
    EventWeightPlugin::weights.resize(1 + 2 * systWeightsIndices.size(), 1.);
}


GenWeightSyst::GenWeightSyst(
  std::initializer_list<std::pair<unsigned, unsigned>> const &systWeightsIndices):
    GenWeightSyst("GenWeightSyst", systWeightsIndices)
{}


GenWeightSyst::GenWeightSyst(std::string const &name, std::string const &weightIndicesFile):
    EventWeightPlugin(name),
    generatorReaderName("Generator"), generatorReader(nullptr),
    referenceWeightIndex(0),
    systWeightsIndicesCurDataset(nullptr),
    rescaleWeights(false),
    meanWeightsCurDataset(nullptr)
{
    // Create a JSON parser and read file with mean weights
    Config config(weightIndicesFile);
    auto const &root = config.Get();
    
    
    // Check top-level structure of the file
    if (not root.isArray())
    {
        std::ostringstream message;
        message << "GenWeightSyst[\"" << GetName() << "\"]::GenWeightSyst: " <<
          "File " << config.FilePath() << " does not contain a list of samples on its top level.";
        throw std::logic_error(message.str());
    }
    
    if (root.size() == 0)
    {
        std::ostringstream message;
        message << "GenWeightSyst[\"" << GetName() << "\"]::GenWeightSyst: " <<
          "List of samples in file " << config.FilePath() << " is empty.";
        throw std::logic_error(message.str());
    }
    
    
    // Loop over all samples included in the file
    for (unsigned iSample = 0; iSample < root.size(); ++iSample)
    {
        auto const &sample = root[iSample];
        
        if (not sample.isObject())
        {
            std::ostringstream message;
            message << "GenWeightSyst[\"" << GetName() << "\"]::GenWeightSyst: " <<
              "Sample #" << iSample << " in file " << config.FilePath() <<
              " does not represent a valid object.";
            throw std::logic_error(message.str());
        }
        
        
        // Read dataset ID
        if (not sample.isMember("datasetId") or not sample["datasetId"].isString())
        {
            std::ostringstream message;
            message << "GenWeightSyst[\"" << GetName() << "\"]::GenWeightSyst: " <<
              "Sample #" << iSample << " in file " << config.FilePath() <<
              " does not contain mandatory field \"datasetId\" or the corresponding value "
              "is not a string.";
            throw std::logic_error(message.str());
        }
        
        std::string const datasetId(sample["datasetId"].asString());
        
        
        // Read weight indices
        std::vector<std::pair<unsigned, unsigned>> readWeightPairs;
        
        if (not sample.isMember("weightPairs") or not sample["weightPairs"].isArray())
        {
            std::ostringstream message;
            message << "GenWeightSyst[\"" << GetName() << "\"]::GenWeightSyst: " <<
              "Sample #" << iSample << " in file " << config.FilePath() <<
              " does not contain mandatory field \"weightPairs\" or the corresponding value "
              "is not an array.";
            throw std::logic_error(message.str());
        }
        
        auto const &weightPairs = sample["weightPairs"];
        
        for (unsigned iPair = 0; iPair < weightPairs.size(); ++iPair)
        {
            auto const &weightPair = weightPairs[iPair];
            
            if (not weightPair.isArray() or weightPair.size() != 2)
            {
                std::ostringstream message;
                message << "GenWeightSyst[\"" << GetName() << "\"]::GenWeightSyst: " <<
                  "Element #" << iPair << " in array \"weightPairs\" for sample #" << iSample <<
                  " in file " << config.FilePath() << " is not an array of size 2.";
                throw std::logic_error(message.str());
            }
            
            if (not weightPair[0].isUInt() or not weightPair[1].isUInt())
            {
                std::ostringstream message;
                message << "GenWeightSyst[\"" << GetName() << "\"]::GenWeightSyst: " <<
                  "Elements of weight pair #" << iPair << " in sample #" << iSample <<
                  " in file " << config.FilePath() << " are not unsigned integer numbers.";
                throw std::logic_error(message.str());
            }
            
            
            readWeightPairs.emplace_back(weightPair[0].asUInt(), weightPair[1].asUInt());
        }
        
        systWeightsIndices.emplace(std::piecewise_construct, std::forward_as_tuple(datasetId),
          std::forward_as_tuple(readWeightPairs));
    }
}


GenWeightSyst::GenWeightSyst(std::string const &weightIndicesFile):
    GenWeightSyst("GenWeightSyst", weightIndicesFile)
{}


GenWeightSyst::GenWeightSyst(GenWeightSyst const &src):
    EventWeightPlugin(src),
    generatorReaderName(src.generatorReaderName),
    generatorReader(nullptr),
    referenceWeightIndex(src.referenceWeightIndex),
    systWeightsIndices(src.systWeightsIndices), systWeightsIndicesCurDataset(nullptr),
    rescaleWeights(src.rescaleWeights),
    meanWeights(src.meanWeights), meanWeightsCurDataset(nullptr)
{}


void GenWeightSyst::BeginRun(Dataset const &dataset)
{
    std::string const &datasetID = dataset.GetSourceDatasetID();
    
    // Save pointer to plugin that provides generator-level weights
    generatorReader =
      dynamic_cast<GeneratorReader const *>(GetDependencyPlugin(generatorReaderName));
    
    
    // Determine what weights should be used for current dataset
    systWeightsIndicesCurDataset = FindWeightIndices(datasetID);
    
    
    
    // Adjust the vector with weights from the base class. The nominal weight will never be
    //modified.
    unsigned const nVars = (systWeightsIndicesCurDataset) ?
      systWeightsIndicesCurDataset->size() : 0;
    
    weights.resize(1 + 2 * nVars);
    std::fill(weights.begin(), weights.end(), 1.);
    
    
    if (systWeightsIndicesCurDataset and rescaleWeights)
    {
        // Save pointer to a map with mean weights for the current dataset. If the map is not
        //found, no systematics will be evaluated, and the number of weights is adjusted
        //accordingly.
        auto const res = meanWeights.find(datasetID);
        
        if (res != meanWeights.end())
        {
            meanWeightsCurDataset = &res->second;
            
            
            // Make sure that there are mean weights for all specified indices
            unsigned missingIndex = std::numeric_limits<unsigned>::max();
            
            if (meanWeightsCurDataset->find(referenceWeightIndex) == meanWeightsCurDataset->end())
                missingIndex = referenceWeightIndex;
            else
            {
                for (auto const &p: *systWeightsIndicesCurDataset)
                {
                    if (meanWeightsCurDataset->find(p.first) == meanWeightsCurDataset->end())
                    {
                        missingIndex = p.first;
                        break;
                    }
                    
                    if (meanWeightsCurDataset->find(p.second) == meanWeightsCurDataset->end())
                    {
                        missingIndex = p.second;
                        break;
                    }
                }
            }
            
            if (missingIndex != std::numeric_limits<unsigned>::max())
            {
                std::ostringstream message;
                message << "GenWeightSyst[\"" << GetName() << "\"]::BeginRun: Mean weight for "
                  "index " << missingIndex << " is not available.";
                throw std::runtime_error(message.str());
            }
        }
        else
        {
            logging::logger << "Warning in GenWeightSyst[\"" << GetName() << "\"]::BeginRun: "
              "When starting processing file " << dataset.GetFiles().front() <<
              " (dataset ID \"" << datasetID << "\") mean weights were not found. All "
              "systematic uncertainties from this plugin will be switched off." << logging::eom;
            
            meanWeightsCurDataset = nullptr;
            weights.resize(1);
            weights[0] = 1.;
        }
    }
    else
        meanWeightsCurDataset = nullptr;
}


Plugin *GenWeightSyst::Clone() const
{
    return new GenWeightSyst(*this);
}


void GenWeightSyst::NormalizeByMeanWeights(std::string const &dbFileName)
{
    // Clear map with mean weights in case this is not the first time this method is called
    meanWeights.clear();
    
    // Memorize that weights need to be rescaled
    rescaleWeights = true;
    
    
    // Create a JSON parser and read file with mean weights
    Config config(dbFileName);
    auto const &root = config.Get();
    
    
    // Check top-level structure of the file
    if (not root.isArray())
    {
        std::ostringstream message;
        message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
          "File \"" << config.FilePath() << "\" does not contain a list of samples on its top level.";
        throw std::logic_error(message.str());
    }
    
    if (root.size() == 0)
    {
        std::ostringstream message;
        message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
          "List of samples in file " << config.FilePath() << " is empty.";
        throw std::logic_error(message.str());
    }
    
    
    // Loop over all samples included in the file
    for (unsigned iSample = 0; iSample < root.size(); ++iSample)
    {
        auto const &sample = root[iSample];
        
        if (not sample.isObject())
        {
            std::ostringstream message;
            message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
              "Sample #" << iSample << " in file " << config.FilePath() <<
              " does not represent a valid object.";
            throw std::logic_error(message.str());
        }
        
        
        // Read dataset ID
        if (not sample.isMember("datasetId") or not sample["datasetId"].isString())
        {
            std::ostringstream message;
            message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
              "Sample #" << iSample << " in file " << config.FilePath() <<
              " does not contain mandatory field \"datasetId\" or the corresponding value "
              "is not a string.";
            throw std::logic_error(message.str());
        }
        
        std::string const datasetId(sample["datasetId"].asString());
        
        
        // Find what weights need to be read for this dataset. If none, continue to the next
        //sample.
        auto const *selectedWeightPairs = FindWeightIndices(datasetId);
        
        if (not selectedWeightPairs)
            continue;
        
        
        // Read mean weights
        if (not sample.isMember("meanLHEWeights") or not sample["meanLHEWeights"].isArray())
        {
            std::ostringstream message;
            message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
              "Sample #" << iSample << " in file " << config.FilePath() <<
              " does not contain mandatory field \"meanLHEWeights\" or the corresponding value "
              "is not an array.";
            throw std::logic_error(message.str());
        }
        
        auto const &weightInfos = sample["meanLHEWeights"];
        
        for (unsigned iWeight = 0; iWeight < weightInfos.size(); ++iWeight)
        {
            auto const &weightInfo = weightInfos[iWeight];
            
            if (not weightInfo.isObject())
            {
                std::ostringstream message;
                message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
                  "Weight #" << iWeight << " in sample #" << iSample << " in file " <<
                  config.FilePath() << " does not represent a valid object.";
                throw std::logic_error(message.str());
            }
            
            
            // Read the index of the current weight
            if (not weightInfo.isMember("index") or not weightInfo["index"].isUInt())
            {
                std::ostringstream message;
                message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
                  "Weight #" << iWeight << " in sample #" << iSample << " in file " <<
                  config.FilePath() << " does not contain mandatory field \"index\" or the "
                  "corresponding value is not an unsigned integer number.";
                throw std::logic_error(message.str());
            }
            
            unsigned const index = weightInfo["index"].asUInt();
            
            
            // Skip to the next weight if the current index is used in the reweighting
            bool indexUsedForSyst = false;
            
            for (auto const &p: *selectedWeightPairs)
            {
                if (p.first == index or p.second == index)
                {
                    indexUsedForSyst = true;
                    break;
                }
            }
            
            if (not indexUsedForSyst and index != referenceWeightIndex)
                continue;
            
            
            // Read the mean weight and store it in the map
            if (not weightInfo.isMember("value") or not weightInfo["value"].isNumeric())
            {
                std::ostringstream message;
                message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
                  "Weight #" << iWeight << " in sample #" << iSample << " in file " <<
                  config.FilePath() << " does not contain mandatory field \"value\" or the "
                  "corresponding value is not a valid number.";
                throw std::logic_error(message.str());
            }
            
            double const meanWeight = weightInfo["value"].asDouble();
            meanWeights[datasetId][index] = meanWeight;
        }
    }
}


std::vector<std::pair<unsigned, unsigned>> const *GenWeightSyst::FindWeightIndices(
  std::string const &datasetID) const
{
    auto const res = systWeightsIndices.find(datasetID);
    
    if (res != systWeightsIndices.end())
        return &res->second;
    else
    {
        // Check if a default exists
        auto const defaultEntryIt = systWeightsIndices.find("*");
        
        if (defaultEntryIt != systWeightsIndices.end())
            return &defaultEntryIt->second;
    }
    
    return nullptr;
}


bool GenWeightSyst::ProcessEvent()
{
    // Do nothing if no weight indices are defined for the current dataset
    if (not systWeightsIndicesCurDataset)
        return true;
    
    // Treat speciall a case when weight renormalization has been requested, but mean weights are
    //not avaliable for the current dataset. Do not compute any systematic weights.
    if (rescaleWeights and not meanWeightsCurDataset)
        return true;
    
    
    // Compute systematic weights. Nominal weight is always 1.
    double const refWeight = generatorReader->GetAltWeight(referenceWeightIndex);
    
    if (meanWeightsCurDataset)
    {
        // Reweighting using weights normalized to their mean values
        double const refWeightMean = meanWeightsCurDataset->at(referenceWeightIndex);
        
        for (unsigned iVar = 0; iVar < systWeightsIndicesCurDataset->size(); ++iVar)
        {
            auto const &indices = (*systWeightsIndicesCurDataset)[iVar];
            
            weights.at(1 + 2 * iVar) =
              (generatorReader->GetAltWeight(indices.first) * refWeightMean) /
              (meanWeightsCurDataset->at(indices.first) * refWeight);
            weights.at(2 + 2 * iVar) =
              (generatorReader->GetAltWeight(indices.second) * refWeightMean) /
              (meanWeightsCurDataset->at(indices.second) * refWeight);
        }
    }
    else if (not rescaleWeights)
    {
        // Reweighting using unnormalized weights
        for (unsigned iVar = 0; iVar < systWeightsIndicesCurDataset->size(); ++iVar)
        {
            auto const &indices = (*systWeightsIndicesCurDataset)[iVar];
            
            weights.at(1 + 2 * iVar) = generatorReader->GetAltWeight(indices.first) / refWeight;
            weights.at(2 + 2 * iVar) = generatorReader->GetAltWeight(indices.second) / refWeight;
        }
    }
    
    
    return true;
}

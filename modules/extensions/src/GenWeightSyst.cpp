#include <mensura/extensions/GenWeightSyst.hpp>

#include <mensura/core/Dataset.hpp>
#include <mensura/core/FileInPath.hpp>
#include <mensura/core/GeneratorReader.hpp>

#include <mensura/external/JsonCpp/json.hpp>

#include <algorithm>
#include <fstream>
#include <limits>
#include <sstream>
#include <stdexcept>


GenWeightSyst::GenWeightSyst(std::string const name,
  std::initializer_list<std::pair<unsigned, unsigned>> const &systWeightsIndices_):
    EventWeightPlugin(name),
    generatorReaderName("Generator"), generatorReader(nullptr),
    referenceWeightIndex(0),
    systWeightsIndices(systWeightsIndices_),
    rescaleWeights(false),
    meanWeightsCurDataset(nullptr)
{
    EventWeightPlugin::weights.resize(1 + 2 * systWeightsIndices.size(), 1.);
}


GenWeightSyst::GenWeightSyst(
  std::initializer_list<std::pair<unsigned, unsigned>> const &systWeightsIndices):
    GenWeightSyst("GenWeightSyst", systWeightsIndices)
{}


GenWeightSyst::GenWeightSyst(GenWeightSyst const &src):
    EventWeightPlugin(src),
    generatorReaderName(src.generatorReaderName),
    generatorReader(nullptr),
    referenceWeightIndex(src.referenceWeightIndex),
    systWeightsIndices(src.systWeightsIndices),
    rescaleWeights(src.rescaleWeights),
    meanWeights(src.meanWeights), meanWeightsCurDataset(nullptr)
{}


void GenWeightSyst::BeginRun(Dataset const &dataset)
{
    // Save pointer to plugin that provides generator-level weights
    generatorReader =
      dynamic_cast<GeneratorReader const *>(GetDependencyPlugin(generatorReaderName));
    
    
    if (rescaleWeights)
    {
        // Save pointer to a map with mean weights for the current dataset. If the map is not
        //found, no systematics will be evaluated, and the number of weights is adjusted
        //accordingly.
        auto const res = meanWeights.find(dataset.GetSourceDatasetID());
        
        if (res != meanWeights.end())
        {
            // Save the pointer and adjust array with weights from the base class. The nominal
            //weight will never be modified.
            meanWeightsCurDataset = &res->second;
            weights.resize(1 + 2 * systWeightsIndices.size());
            std::fill(weights.begin(), weights.end(), 1.);
            
            
            // Make sure that there are mean weights for all specified indices
            unsigned missingIndex = std::numeric_limits<unsigned>::max();
            
            if (meanWeightsCurDataset->find(referenceWeightIndex) == meanWeightsCurDataset->end())
                missingIndex = referenceWeightIndex;
            else
            {
                for (auto const &p: systWeightsIndices)
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
    std::string const resolvedPath(FileInPath::Resolve(dbFileName));
    std::ifstream dbFile(resolvedPath, std::ifstream::binary);
    Json::Value root;
    
    try
    {
        dbFile >> root;
    }
    catch (Json::Exception const &)
    {
        std::ostringstream message;
        message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
          "Failed to parse file \"" << resolvedPath << "\". It is not a valid JSON file or is "
          "corrupted.";
        throw std::runtime_error(message.str());
    }
    
    dbFile.close();
    
    
    // Check top-level structure of the file
    if (not root.isArray())
    {
        std::ostringstream message;
        message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
          "File \"" << resolvedPath << "\" does not contain a list of samples on its top level.";
        throw std::logic_error(message.str());
    }
    
    if (root.size() == 0)
    {
        std::ostringstream message;
        message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
          "List of samples in file \"" << resolvedPath << "\" is empty.";
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
              "Sample #" << iSample << " in file \"" << resolvedPath <<
              "\" does not represent a valid object.";
            throw std::logic_error(message.str());
        }
        
        
        // Read dataset ID
        if (not sample.isMember("datasetId") or not sample["datasetId"].isString())
        {
            std::ostringstream message;
            message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
              "Sample #" << iSample << " in file \"" << resolvedPath <<
              "\" does not contain mandatory field \"datasetId\" or the corresponding value "
              "is not a string.";
            throw std::logic_error(message.str());
        }
        
        std::string const datasetId(sample["datasetId"].asString());
        
        
        // Read mean weights
        if (not sample.isMember("meanLHEWeights") or not sample["meanLHEWeights"].isArray())
        {
            std::ostringstream message;
            message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
              "Sample #" << iSample << " in file \"" << resolvedPath <<
              "\" does not contain mandatory field \"meanLHEWeights\" or the corresponding value "
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
                  "Weight #" << iWeight << "in sample #" << iSample << " in file \"" <<
                  resolvedPath << "\" does not represent a valid object.";
                throw std::logic_error(message.str());
            }
            
            
            // Read the index of the current weight
            if (not weightInfo.isMember("index") or not weightInfo["index"].isUInt())
            {
                std::ostringstream message;
                message << "GenWeightSyst[\"" << GetName() << "\"]::NormalizeByMeanWeights: " <<
                  "Weight #" << iWeight << "in sample #" << iSample << " in file \"" <<
                  resolvedPath << "\" does not contain mandatory field \"index\" or the "
                  "corresponding value is not an unsigned integer number.";
                throw std::logic_error(message.str());
            }
            
            unsigned const index = weightInfo["index"].asUInt();
            
            
            // Skip to the next weight if the current index is used in the reweighting
            bool indexUsedForSyst = false;
            
            for (auto const &p: systWeightsIndices)
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
                  "Weight #" << iWeight << "in sample #" << iSample << " in file \"" <<
                  resolvedPath << "\" does not contain mandatory field \"value\" or the "
                  "corresponding value is not a valid number.";
                throw std::logic_error(message.str());
            }
            
            double const meanWeight = weightInfo["value"].asDouble();
            meanWeights[datasetId][index] = meanWeight;
        }
    }
}


bool GenWeightSyst::ProcessEvent()
{
    // Treat speciall a case when weight renormalization has been requested, but mean weights are
    //not avaliable for the current dataset. Do not compute any systematic weights.
    if (rescaleWeights and not meanWeightsCurDataset)
        return true;
    
    
    // Compute systematic weights. Nominal weight is always 1.
    double const refWeight = generatorReader->GetAltWeight(referenceWeightIndex);
    
    if (meanWeightsCurDataset)
    {
        // Reweighting using weights normalized to their mean values
        for (unsigned iVar = 0; iVar < systWeightsIndices.size(); ++iVar)
        {
            auto const &indices = systWeightsIndices[iVar];
            double const refWeightMean = meanWeightsCurDataset->at(referenceWeightIndex);
            
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
        for (unsigned iVar = 0; iVar < systWeightsIndices.size(); ++iVar)
        {
            auto const &indices = systWeightsIndices[iVar];
            
            weights.at(1 + 2 * iVar) = generatorReader->GetAltWeight(indices.first) / refWeight;
            weights.at(2 + 2 * iVar) = generatorReader->GetAltWeight(indices.second) / refWeight;
        }
    }
    
    
    return true;
}

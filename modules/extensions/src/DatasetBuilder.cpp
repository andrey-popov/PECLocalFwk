#include <mensura/extensions/DatasetBuilder.hpp>

#include <mensura/core/FileInPath.hpp>

#include <fstream>
#include <memory>
#include <sstream>
#include <stdexcept>


DatasetBuilder::DatasetBuilder(std::string const &dbSampleFileName)
{
    // Parse the database file
    std::string const resolvedPath(FileInPath::Resolve(dbSampleFileName));
    std::ifstream dbFile(resolvedPath, std::ifstream::binary);
    Json::Value root;
    
    try
    {
        dbFile >> root;
    }
    catch (Json::Exception const &)
    {
        std::ostringstream message;
        message << "DatasetBuilder::DatasetBuilder: " <<
          "Failed to parse file \"" << resolvedPath << "\". It is not a valid JSON file, or "
          "the file is corrupted.";
        throw std::runtime_error(message.str());
    }
    
    dbFile.close();
    
    
    // Basic sanity check
    if (not root.isArray())
    {
        std::ostringstream message;
        message << "DatasetBuilder::DatasetBuilder: " <<
          "File \"" << resolvedPath << "\" does not contain a list of datasets on its top level.";
        throw std::logic_error(message.str());
    }
    
    if (root.size() == 0)
    {
        std::ostringstream message;
        message << "DatasetBuilder::DatasetBuilder: " <<
          "List of datasets in file \"" << resolvedPath << "\" is empty.";
        throw std::logic_error(message.str());
    }
    
    
    // Read information about all datasets
    for (unsigned iSample = 0; iSample < root.size(); ++iSample)
    {
        auto const &sample = root[iSample];
        
        if (not sample.isObject())
        {
            std::ostringstream message;
            message << "DatasetBuilder::DatasetBuilder: " <<
              "Entry #" << iSample << " in file \"" << resolvedPath <<
              "\" does not represent a valid object.";
            throw std::logic_error(message.str());
        }
        
        
        // Read dataset ID
        if (not sample.isMember("datasetId") or not sample["datasetId"].isString())
        {
            std::ostringstream message;
            message << "DatasetBuilder::DatasetBuilder: " <<
              "Entry #" << iSample << " in file \"" << resolvedPath <<
              "\" does not contain mandatory field \"datasetId\", or the corresponding value "
              "is not a string.";
            throw std::logic_error(message.str());
        }
        
        std::string const datasetID(sample["datasetId"].asString());
        
        
        // Save information about the current sample in a map
        dbSamples[datasetID] = sample;
    }
    
    
    // Set directory from which paths to input files will be resolved. It is initialized to the
    //directory containing the database file
    auto pos = resolvedPath.find_last_of('/');
    
    if (pos == std::string::npos)
        baseDirectory = "";
    else
        baseDirectory = resolvedPath.substr(0, pos + 1);
}


std::list<Dataset> DatasetBuilder::Build(std::initializer_list<std::string> const &datasetIDs)
  const
{
    std::list<Dataset> datasets;
    
    for (auto const &datasetID: datasetIDs)
    {
        // Find details for the current dataset ID
        auto entryIt = dbSamples.find(datasetID);
        
        if (entryIt == dbSamples.end())
        {
            std::ostringstream message;
            message << "DatasetBuilder::Build: Requested dataset ID \"" << datasetID <<
              "\" is not found in the database.";
            throw std::runtime_error(message.str());
        }
        
        auto const &sample = entryIt->second;
        
        
        // Read the type of the dataset and the list of files
        if (not sample.isMember("isData") or not sample["isData"].isBool())
        {
            std::ostringstream message;
            message << "DatasetBuilder::Build: Entry for dataset ID \"" << datasetID <<
              "\" does not contain mandatory field \"isData\", or the corresponding value is " <<
              "not a boolean.";
            throw std::logic_error(message.str());
        }
        
        bool const isData = sample["isData"].asBool();
        
        
        if (not sample.isMember("files") or not sample["files"].isArray())
        {
            std::ostringstream message;
            message << "DatasetBuilder::Build: Entry for dataset ID \"" << datasetID <<
              "\" does not contain mandatory field \"files\", or it is not an array.";
            throw std::logic_error(message.str());
        }
        
        auto const &fileArray = sample["files"];
        std::list<std::string> filePaths;
        
        for (unsigned i = 0; i < fileArray.size(); ++i)
        {
            std::string const extractedPath(fileArray[i].asString());
            
            if (extractedPath.empty())
            {
                std::ostringstream message;
                message << "DatasetBuilder::Build: Entry for dataset ID \"" << datasetID <<
                  "\" contains an empty path in array \"files\".";
                throw std::logic_error(message.str());
            }
            
            if (extractedPath[0] == '/')
            {
                // This is an absolute path. Use it as is
                filePaths.emplace_back(extractedPath);
            }
            else
            {
                // This is a relative path. Resolve it with respect to the standard location
                filePaths.emplace_back(baseDirectory + extractedPath);
            }
        }
        
        
        // Create the dataset
        if (isData)
        {
            Dataset dataset(Dataset::Type::Data, datasetID);
            
            for (auto const &filePath: filePaths)
                dataset.AddFile(filePath);
            
            datasets.emplace_back(std::move(dataset));
        }
        else
        {
            // Read information for normalization
            if (not sample.isMember("crossSection") or not sample["crossSection"].isNumeric())
            {
                std::ostringstream message;
                message << "DatasetBuilder::Build: Entry for dataset ID \"" << datasetID <<
                  "\" does not contain field \"crossSection\", or the corresponding value is "
                  "not numeric.";
                throw std::logic_error(message.str());
            }
            
            double const crossSection = sample["crossSection"].asDouble();
            
            if (not sample.isMember("eventsProcessed") or
              not sample["eventsProcessed"].isNumeric())
            {
                std::ostringstream message;
                message << "DatasetBuilder::Build: Entry for dataset ID \"" << datasetID <<
                  "\" does not contain field \"eventsProcessed\", or the corresponding value is "
                  "not numeric.";
                throw std::logic_error(message.str());
            }
            
            unsigned long const eventsProcessed = sample["eventsProcessed"].asLargestUInt();
            
            
            double meanWeight = 1.;
            
            if (sample.isMember("meanWeight"))
            {
                if (not sample["meanWeight"].isNumeric())
                {
                    std::ostringstream message;
                    message << "DatasetBuilder::Build: Entry for dataset ID \"" << datasetID <<
                      "\" contains field \"meanWeight\" which is not of a numeric type.";
                    throw std::logic_error(message.str());
                }
                
                meanWeight = sample["meanWeight"].asDouble();
            }
            
            
            // Now can create the dataset
            Dataset dataset(Dataset::Type::MC, datasetID);
            
            for (auto const &filePath: filePaths)
                dataset.AddFile(filePath, crossSection, eventsProcessed, meanWeight);
            
            datasets.emplace_back(std::move(dataset));
        }
    }
    
    
    return datasets;
}


void DatasetBuilder::SetBaseDirectory(std::string const &path)
{
    baseDirectory = path;
}

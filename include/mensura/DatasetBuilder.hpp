#pragma once

#include <mensura/Dataset.hpp>

#include <filesystem>
#include <initializer_list>
#include <list>
#include <map>
#include <string>


namespace Json {
    class Value;
};


/**
 * \class DatasetBuilder
 * \brief A class to automatize construction of datasets using a JSON file with descriptions
 * 
 * The purpose of this class is to relief user from typing in properties of datasets manually.
 * Instead they are read from a JSON file, and datasets to be constructed are identified simply by
 * their IDs. A list of IDs is given as input to method Build (or its shortcuts), and it produces
 * a list of fully constructed instances of class Dataset.
 * 
 * Input JSON file should have the following structure:
 * \code{.json}
 *   [
 *     {
 *       "datasetId": "SingleMuon-Run2016B_320_rjq",
 *       "files": ["SingleMuon-Run2016B_320_rjq.part*.root"],
 *       "isData": true
 *     },
 *     {
 *       "datasetId": "ttbar-pw_320_hFE",
 *       "files": ["ttbar-pw_320_hFE.part*.root"],
 *       "isData": false,
 *       "crossSection": 831.76,
 *       "eventsProcessed": 92925926,
 *       "meanWeight": 1.
 *     },
 *   ]
 * \endcode
 * The first entry is an example for experimental data, the second one is for simulation. Comments
 * are allowed.
 * 
 * Unless JSON file gives absolute paths for input files, they are resolved with respect to the
 * directory containing the JSON file or directory specified explicitly via SetBaseDirectory.
 */
class DatasetBuilder
{
public:
    /**
     * \brief Constructor from a JSON file
     * 
     * Performs a basic validation of the JSON file and loads descriptions of all datasets.
     * Directory that contains the file is set as the base directory.
     */
    DatasetBuilder(std::string const &dbSampleFileName);

    ~DatasetBuilder() noexcept;
    
public:
    /**
     * \brief Constructs a list of datasets based on the given dataset IDs
     * 
     * Paths to files in the datasets are resolved with respect to the base directory initialized
     * in the constructor or given explicitly by method SetBaseDirectory.
     */
    std::list<Dataset> Build(std::initializer_list<std::string> const &datasetIDs) const;
    
    /// Shortcut for namesake method
    std::list<Dataset> Build(std::string const &datasetID) const
    {
        return Build({datasetID});
    }
    
    /// Shortcut for method Build
    std::list<Dataset> operator()(std::initializer_list<std::string> const &datasetIDs) const
    {
        return Build(datasetIDs);
    }
    
    /// Shortcut for method Build
    std::list<Dataset> operator()(std::string const &datasetID) const
    {
        return Build({datasetID});
    }
    
    /// Changes the directory with respect to which paths to input files are resolved
    void SetBaseDirectory(std::filesystem::path const &path);
    
private:
    /**
     * \brief Parsed JSON descriptions of samples
     * 
     * The key of the map is the corresponding dataset ID.
     */
    std::map<std::string, Json::Value> dbSamples;
    
    /// Directory from which paths to files are resolved
    std::filesystem::path baseDirectory;
};

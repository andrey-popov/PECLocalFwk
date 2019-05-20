#pragma once

#include <string>
#include <list>
#include <unordered_set>


/**
 * \class Dataset
 * \brief A class to store information about a dataset
 * 
 * This class aggregates all basic properties of a dataset, most notably it contains a list of
 * input ROOT files together with information needed for normalization of simulated datasets
 * (cross sections, total numbers of processed events, and mean weights). A flag distinguishing
 * between experimental data and simulation is also stored.
 * 
 * Each dataset must be assigned an arbitrary label that uniquely identifies the source dataset
 * from which the files were produced. Physics content of files within the same dataset or multiple
 * datasets with identical labels is the same. If user does not specify this label, referred to as
 * source dataset ID, it is deduced automatically from the name of the first input file.
 * 
 * User can add arbitrary boolean flags to provide additional information about the dataset to
 * custom plugins.
 */
class Dataset
{
public:
    /**
     * \struct File
     * \brief Aggregates path to a file, cross section, number of events in the parent dataset, and
     * mean generator-level weight
     */
    struct File
    {
        /// Default constructor
        File() noexcept;
        
        /**
         * \brief Constructor from file name only (to be used for real data)
         * 
         * This constructor is made explicit in order to avoid ambiguity in Dataset::AddFile.
         */
        explicit File(std::string const &name) noexcept;
        
        /// Simple initializing constructor
        File(std::string const &name, double xSec, unsigned long nEvents, double meanWeight = 1.)
          noexcept;
        
        /// Returns file basename with stripped extension
        std::string GetBaseName() const;
        
        /// Returns name of directory containing the file
        std::string GetDirName() const;
        
        /**
         * \brief Computes event weight to obtain correct normalization in simulation
         * 
         * The weight is computed as xSec / (meanWeight * nEvents) and corresponds to normalization
         * of simulation to an integrated luminosity of 1/pb. The result is meaningless in case of
         * experimental data.
         */
        double GetWeight() const;
        
        /// Fully-qualified file name
        std::string name;
        
        /// Cross section in pb
        double xSec;
        
        /// Number of events in the parent dataset
        unsigned long nEvents;
        
        /// Mean generator-leve weight
        double meanWeight;
    };
    
    /**
     * \enum Type
     * \brief A type to distinguish collision data and simulation
     */
    enum class Type
    {
        Data,
        MC
    };

public:
    /// Constructor with no parameters
    Dataset() noexcept;
    
    /**
     * \brief Create an empty dataset with the given type and source dataset ID
     * 
     * If an empty label is given to the source dataset ID, it will be deduced from the name of the
     * first file when added.
     */
    Dataset(Type type, std::string sourceDatasetID = "");
    
    /// Default copy constructor
    Dataset(Dataset const &) = default;
    
    /// Default move constructor
    Dataset(Dataset &&) noexcept = default;
    
    /// Default assignment operator
    Dataset &operator=(Dataset const &) = default;

public:
    /**
     * \brief Adds a new file to the list
     * 
     * The file name part of the given path can contain wildcards '*' and '?'. Consult
     * documentation for ExpandPathMask for details.
     */
    void AddFile(std::string const &path, double xSec, unsigned long nEvents,
      double meanWeight = 1.);
    
    /// A short-cut of the above method to be used with data
    void AddFile(std::string const &path);
    
    /**
     * \brief Adds a new file to the list
     */
    void AddFile(File const &file) noexcept;
    
    /**
     * \brief Returns the list of the files
     */
    std::list<File> const &GetFiles() const;
    
    /// Returns label that uniquely identifies the source dataset
    std::string const &GetSourceDatasetID() const;
    
    /**
     * \brief Checks if the dataset corresponds to the simulation and not the real data
     * 
     * Note that the discrimination is performed based on the value of the first process code, and
     * if it is undefined, then method returns true.
     */
    bool IsMC() const;
    
    /// Creates a clone of this dataset with an empty file list
    Dataset CopyParameters() const;
    
    /**
     * \brief Sets a flag with given name
     * 
     * If a flag with given name has already been registered, an exception is thrown. Note that
     * internally an unordered_set is used to store names of flags, and, therefore, the flags
     * are compared with the help of std::hash<std::string> function, which is not guaranteed
     * to produce different hash values for two strings that differ well according to human
     * intuition.
     */
    void SetFlag(std::string const &flagName);
    
    /**
     * \brief Unsets the flag with given name
     * 
     * If the flag is not found, method takes no effect.
     */
    void UnsetFlag(std::string const &flagName);
    
    /// Tests if the flag with given name is set
    bool TestFlag(std::string const &flagName) const;
    
private:
    /**
     * \brief Returns paths to files whose names match the provided mask
     * 
     * If the given path mask contains symbols '*' or '?' in the file name, files in the enclosing
     * directory are listed and paths to whose matching the mask are returned. No wildcards are
     * allowed in the path to the directory (i.e. before that last slash in the given mask). If no
     * files match the mask, an exception is thrown.
     * 
     * If the path mask does not contain wildcards, it is returned without modifications. In this
     * case the method does not verify that the file exists.
     */
    static std::list<std::string> ExpandPathMask(std::string const &path);
    
    /**
     * \brief Sets source dataset ID based on the name of the last added file
     * 
     * Optional postfix with the part number is stripped off if present.
     */
    void SetDefaultSourceDatasetID();
    
private:
    /// Source files
    std::list<File> files;
    
    /// A label that uniquely identifies the source dataset
    std::string sourceDatasetID;
    
    /// Indicates whether this dataset is data or simulation
    bool isData;
    
    /// A facility to emulate user-defined flags
    std::unordered_set<std::string> flags;
};

/**
 * \file Dataset.hpp
 * 
 * The module defines a class to descibe a dataset.
 */

#pragma once

#include <string>
#include <list>
#include <unordered_set>


/**
 * \class Dataset
 * \brief A class to store information about a dataset
 * 
 * The class aggregates all relevant information about a dataset. It includes names of ROOT files,
 * corresponding cross sections and numbers of events in the parent datasets processed in Grid. It
 * also attaches one or more codes to specify physics process represendted by the dataset. If
 * several process codes are provided, they describe the process at different levels of
 * detalisation, e.g. ttbar and semileptonic ttbar; they are never scrict synonyms. In addition,
 * information about generators of the hard process and the showering can be specified.
 * 
 * Moreover, the user can define arbitrary boolean flags binded with an instance of the class. Their
 * usage is not governed by the framework.
 */
class Dataset
{
public:
    /**
     * \struct File
     * \brief Contains the file name, the cross section, and the number of events in the parent
     * dataset
     */
    struct File
    {
        /// Default constructor
        File() noexcept;
        
        /// Constructor from file name only (to be used for real data)
        File(std::string const &name) noexcept;
        
        /// Simple initializing constructor
        File(std::string const &name, double xSec, unsigned long nEvents) noexcept;
        
        /// Returns file basename with stripped extension
        std::string GetBaseName() const noexcept;
        
        /// Returns name of directory containing the file
        std::string GetDirName() const noexcept;
        
        std::string name;  ///< Fully-qualified file name
        double xSec;  ///< Cross section in pb
        unsigned long nEvents;  ///< Number of events in the parent dataset
    };
    
    /**
     * \enum Generator
     * \brief Defines the supported generators for the hard process
     */
    enum class Generator
    {
        Undefined,
        Nature,
        Pythia,
        MadGraph,
        aMCatNLO,
        POWHEG,
        CompHEP,
        SHERPA
    };
    
    /**
     * \enum ShowerGenerator
     * \brief Parton shower and hadronization generator
     */
    enum class ShowerGenerator
    {
        Undefined,
        Nature,
        Pythia,
        Herwig
    };
    
    /**
     * \enum Process
     * \brief Code to describe physics process represented by the dataset
     * 
     * Specialised process codes must always be described after the corresponding more general codes
     * so that when the enumeration values are converted into integer numbers more general
     * categories get smaller numbers.
     */
    enum class Process
    {
        Undefined,
        
        ppData,       ///< Generic category for any real pp collisions
        pp7TeV,
        pp8TeV,
        pp13TeV,
        
        BSM,          ///< Common category for BSM signals
        
        tHq,          ///< tHq with any couplings
        tHqExotic,    ///< tHq with kappa_t = -1
        tHqSM,        ///< Standard-Model tHq (kappa_t = +1)
        
        ZPrime,
        WPrime,
        
        ttbar,        ///< Generic category for any ttbar dataset
        ttInclusive,  ///< Inclusive ttbar, i.e. the dataset contains any decays
        ttSemilep,    ///< Exclusive semileptonic ttbar
        ttDilep,
        ttHad,        ///< Exclusive hadronic ttbar
        
        SingleTop,    ///< Generic category to describe any single-top process
        ttchan,
        tschan,
        ttWchan,
        
        ttH,
        
        EWK,          ///< Generic category for production of W and Z bosons
        Wjets,
        Diboson,      ///< WW, WZ, or ZZ
        DrellYan,
        
        QCD,
        Photon        ///< Prompt-photon production
    };

public:
    /// Constructor with no parameters
    Dataset() noexcept;
    
    /**
     * \brief Constructor with parameters
     * 
     * A dataset can be assigned more than one process code. In this case the codes must be
     * logically compatible (a dataset cannot be both Wjets and DrellYan). If a specialised code is
     * used, the most general code of the category must also be specified. For example, if a dataset
     * is semileptonic ttbar, it must be described as (generic) ttbar as well.
     * 
     * By default, generator and shower generator are set to Undefined. However, if the process
     * is real data, they are changed to Nature.
     */
    Dataset(std::list<Process> &&processCodes, Generator generator = Generator::Undefined,
     ShowerGenerator showerGenerator = ShowerGenerator::Undefined) noexcept;
    
    /**
     * \brief Constructor with parameters
     * 
     * A specialised version used when the list of process codes cannot be given as an rvalue.
     * Refer to documentation of the first version of constructor for details.
     */
    Dataset(std::list<Process> const &processCodes, Generator generator = Generator::Undefined,
     ShowerGenerator showerGenerator = ShowerGenerator::Undefined) noexcept;
    
    /**
     * \brief Constructor with parameters
     * 
     * This version is intended for backward compatibility and for a bit of syntax sugar when a
     * dataset is assigned a single process code. Refer to documentation of the first version of
     * constructor for details.
     */
    Dataset(Process process, Generator generator = Generator::Undefined,
     ShowerGenerator showerGenerator = ShowerGenerator::Undefined) noexcept;
    
    /// Default copy constructor
    Dataset(Dataset const &) = default;
    
    /// Move constructor
    Dataset(Dataset &&src) noexcept;
    
    /// Default assignment operator
    Dataset &operator=(Dataset const &) = default;

public:
    /**
     * \brief Adds a new file to the list
     */
    void AddFile(std::string const &name, double xSec, unsigned long nEvents) noexcept;
    
    /**
     * \brief Adds a new file to the list
     */
    void AddFile(File const &file) noexcept;
    
    /**
     * \brief Returns the list of the files
     */
    std::list<File> const &GetFiles() const;
    
    /**
     * \brief Returns the hard-process generator
     */
    Generator GetGenerator() const;
    
    /**
     * \brief Returns parton-shower and hadronization generator
     */
    ShowerGenerator GetShowerGenerator() const;
    
    /**
     * \brief Returns the most specialised process code
     * 
     * Technically, the last process code is returned because the list is ordered from most general
     * to most specialised specification.
     */
    Process GetProcess() const;
    
    /// Return a list of all assigned process codes
    std::list<Process> const &GetProcessCodes() const;
    
    /// Tests if the given process code is assigned to the dataset
    bool TestProcess(Process code) const;
    
    /**
     * \brief Checks if the dataset corresponds to the simulation and not the real data
     * 
     * Note that the discrimination is performed based on the value of the first process code, and
     * if it is undefined, then method returns true.
     */
    bool IsMC() const;
    
    /**
     * \brief Creates a new instance of Dataset with exactly the same parameters but with an
     * empty file list
     */
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
    /// Orders process codes from most general to most specialised
    static std::list<Process> SortProcessCodes(std::list<Process> &&processCodes);
    
    /**
     * \brief Orders process codes from most general to most specialised
     * 
     * Internally it calls the above version with rvalue reference.
     */
    static std::list<Process> SortProcessCodes(std::list<Process> const &processCodes);
    
    /**
     * \brief Initialisation block called from each constructor
     * 
     * If generator of showerGenerator are undefined and the process is real data, sets the
     * generator codes appropriately.
     */
    void Init();
    
private:
    /// Source files
    std::list<File> files;
    
    /**
     * \brief Description of the physical process
     * 
     * If several process codes are provided, they are ordered following the integer representation
     * of the Process enumeration.
     */
    std::list<Process> processCodes;
    
    /// Generator of the hard interaction
    Generator generator;
    
    /// Shower generator
    ShowerGenerator showerGenerator;
    
    /// A facility to emulate user-defined flags
    std::unordered_set<std::string> flags;
};

/**
 * \file Dataset.hpp
 * \author Andrey Popov
 * 
 * The module defines a class to store all the relevant information about a dataset.
 */

#pragma once

#include <DatasetForward.hpp>

#include <string>
#include <list>
#include <unordered_set>


/**
 * \class Dataset
 * \brief A class to store information about a dataset
 * 
 * The class stores all the relevant information to describe a dataset. It includes the file name,
 * the cross-section and number of events in the parent dataset processed in GRID, the ID of the
 * physical process and the generator, etc. The user can also define arbitrary boolean flags.
 */
class Dataset
{
    public:
        /**
         * \struct File
         * \brief Contains the file name, the cross-section, and the number of events in the parent
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
            
            std::string name;  ///< The full file name
            double xSec;  ///< The cross-section in pb
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
         * \brief Physical process described by the dataset
         */
        enum class Process
        {
            Undefined,
            pp7TeV,
            pp8TeV,
            thqExotic,
            thqSM,
            ttbar,
            ttchan,
            tschan,
            ttWchan,
            tth,
            Wjets,
            Diboson,
            DrellYan,
            QCD
        };
    
    public:
        /// Default constructor
        Dataset() noexcept;
        
        /// Constructor with parameters
        Dataset(Process process, Generator generator, ShowerGenerator showerGenerator) noexcept;
    
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
         * \brief Get the list of the files
         */
        std::list<File> const &GetFiles() const;
        
        /**
         * \brief Get the hard-process generator
         */
        Generator GetGenerator() const;
        
        /**
         * \brief Get parton-shower and hadronization generator
         */
        ShowerGenerator GetShowerGenerator() const;
        
        /**
         * \brief Get the physical process
         */
        Process GetProcess() const;
        
        /**
         * \brief Checks if the dataset corresponds to the simulation and not the real data
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
        std::list<File> files;  ///< Source files
        Process process;  ///< Physical process
        Generator generator;  ///< Generator of the hard interaction
        ShowerGenerator showerGenerator;  ///< Shower generator
        
        /// A facility to emulate user-defined flags
        std::unordered_set<std::string> flags;
};
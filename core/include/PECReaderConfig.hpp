/**
 * \file PECReaderConfig.hpp
 * \author Andrey Popov
 * 
 * Module defines a class to forward parameters to an instance of PECReader.
 */

#pragma once

#include <PECReaderConfigForward.hpp>

#include <TriggerSelectionInterface.hpp>
#include <EventSelectionInterface.hpp>
#include <BTagger.hpp>
#include <BTagDatabase.hpp>
#include <WeightPileUpInterface.hpp>
#include <SystDefinition.hpp>

#include <memory>
#include <string>


/**
 * \class PECReaderConfig
 * \brief Aggregates input parameters for PECReader class
 * 
 * The class aggregates all the configurations for an instance of PECReader. Its presence is
 * motivated by the fact that parameters for PECReader must be handled through an additional layer
 * of interface when the plugin system is used (implemented with classes RunManager and Processor).
 * 
 * When configuration parameters are reperesented by composite objects, they are copied (usually
 * with the help of polymorphic cloning functionality); instances provided by the user are never
 * kept. The copies are either owned by this or shared among all the copies of this (depending on
 * the meaning of the configuration module). Getters are provided to retrieve constance pointers
 * to these objects. For selected ones also non-constant pointers can be retrieved (when the logic
 * of a module's workflow requires call for non-constant methods). When this is copied, all the
 * configuration modules are cloned appropriately.
 * 
 * None of aggregated configuration parameters is considered mandatory, but if an undefined
 * parameter is accessed, an exception is thrown. Parameters of integral types are never considered
 * undefined. For selected parameters dedicated methods are provided to check if the parameters are
 * defined.
 * 
 * The class is not thread-safe. Design of the framework assumes that each instance of class
 * PECReader is accompanied by a copy of PECReaderConfig; therefore, there is a copy of the object
 * in each thread.
 * 
 * The class is copyable.
 */
class PECReaderConfig
{
    public:
        /// Constructor with no parameters
        PECReaderConfig();
        
        /// Copy constructor
        PECReaderConfig(PECReaderConfig const &src);
        
        /// Assignment operator is deleted
        PECReaderConfig &operator=(PECReaderConfig const &) = delete;
    
    
    public:
        /**
         * \brief Specifies trigger selection
         * 
         * Provided object is cloned.
         */
        void SetTriggerSelection(TriggerSelectionInterface *triggerSelection);
        
        /// A short-cut for SetTriggerSelection
        void SetModule(TriggerSelectionInterface *triggerSelection);
        
        /**
         * \brief Specifies offline event selection
         * 
         * Provided object is cloned.
         */
        void SetEventSelection(EventSelectionInterface const *eventSelection);
        
        /// A short-cut for SetEventSelection
        void SetModule(EventSelectionInterface const *eventSelection);
        
        /**
         * \brief Provides b-tagging object
         * 
         * The object is copied.
         */
        void SetBTagger(BTagger const *bTagger);
        
        /// A short-cut for SetBTagger
        void SetModule(BTagger const *bTagger);
        
        /**
         * \brief Provides a handle to a database with b-tagging information
         * 
         * Provided object is copied.
         */
        void SetBTagDatabase(BTagDatabase const *bTagDatabase);
        
        /// A short-cut for SetBTagDatabase
        void SetModule(BTagDatabase const *bTagDatabase);
        
        /**
         * \brief Sets an object to reweight pile-up in simulation
         * 
         * Provided object is copied.
         */
        void SetPileUpReweighter(WeightPileUpInterface const *puReweighter);
        
        /// A short-cut for SetPileUpReweighter
        void SetModule(WeightPileUpInterface const *puReweighter);
        
        /// Sets path to directory containing weight files
        void SetWeightFilesLocation(std::string const &weightFilesLocation);
        
        /**
         * \brief Specifies whether generator information about hard interaction should be read
         * 
         * This setting is ignored for real data.
         */
        void SetReadHardInteraction(bool readHardInteraction);
        
        /// Specifies desired systematical variation
        void SetSystematics(SystVariation const &syst);
        
        
        /// Checks if a valid trigger selection is set
        bool IsSetTriggerSelection() const;
        
        /// Consult documentation for SetTriggerSelection for details
        TriggerSelectionInterface *GetTriggerSelection() const;
        
        /// Checks if a valid event selection is set
        bool IsSetEventSelection() const;
        
        /// Consult documentation for SetEventSelection for details
        EventSelectionInterface const *GetEventSelection() const;
        
        /// Check if a valid b-tagging object is set
        bool IsSetBTagger() const;
        
        /// Consult documentation for SetBTagger for details
        BTagger const *GetBTagger() const;
        
        /// Check if a valid b-tagging database object is set
        bool IsSetBTagDatabase() const;
        
        /// Consult documentation for SetBTagDatabase for details
        BTagDatabase const *GetBTagDatabase() const;
        
        /// Consult documentation for SetBTagDatabase for details
        BTagDatabase *GetBTagDatabase();
        
        /// Checks if a valid pile-up reweighting object is set
        bool IsSetPileUpReweighter() const;
        
        /// Consult documentation for SetPileUpReweigter for details
        WeightPileUpInterface const *GetPileUpReweighter() const;
        
        /// Consult documentation for SetPileUpReweigter for details
        WeightPileUpInterface *GetPileUpReweighter();
        
        /// Checks if a valid path to directory with weight files is set
        bool IsSetWeightFilesLocation() const;
        
        /// Consult documentation for SetWeightFilesLocation for details
        std::string const &GetWeightFilesLocation() const;
        
        /// Consult documentation for SetReadHardInteraction for details
        bool GetReadHardInteraction() const;
        
        /// Consult documentation for SetSystematics for details
        SystVariation const &GetSystematics() const;
    
    
    private:
        /// Trigger selection
        std::unique_ptr<TriggerSelectionInterface> triggerSelection;
        
        /// Event selection
        std::unique_ptr<EventSelectionInterface> eventSelection;
        
        /**
         * \brief An object to define b-tagged jets
         * 
         * The object is shared among all copies of this.
         */
        std::shared_ptr<BTagger const> bTagger;
        
        /// Database with b-tagging efficiencies and scale factors
        std::unique_ptr<BTagDatabase> bTagDatabase;
        
        /// An object to reweight simulation over pile-up
        std::unique_ptr<WeightPileUpInterface> puReweighter;
        
        /// Directory that contains weight files. The string ends with a slash
        std::string weightFilesLocation;
        
        /**
         * \brief Flag specifying whether generator information about hard interaction should be
         * read
         * 
         * This setting is ignored for real data
         */
        bool readHardInteraction;
        
        /// Requested systematical variation
        SystVariation syst;
};
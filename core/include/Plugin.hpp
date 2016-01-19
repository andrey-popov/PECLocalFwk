/**
 * \file Plugin.hpp
 * 
 * The module describes an interface for a plugin for Processor.
 */

#pragma once

#include <ProcessorForward.hpp>
#include <DatasetForward.hpp>

#include <string>


/**
 * \class Plugin
 * \brief An abstract class to define a plugin to be used in class Processor
 * 
 * The class specifies a simple interface that allows to perform a certain processing for each event
 * in a dataset and, additionally, notify the plugin when processing of the dataset starts and
 * finishes. It contains a pointer to the parent Processor instance that can be used to access
 * other plugins attached to the same Processor.
 * 
 * A single plugin might be used to process several datasets, which requires certain initialization
 * and termination actions to be performed in BeginRun/EndRun methods in case of non-trivial
 * plugins. The pointer to the parent Processor instance is initialized before processing of the
 * first dataset starts and is expected to be valid during the lifetime of the plugin.
 * 
 * A derived class might or might not be copyable, but it must implement a method to clone an
 * instance. The clonning must address only configuration of the processing algorithm but not data
 * members specific for a dataset or an event (e.g. handlers of output files). Such a functionality
 * is required to multiplicate the plugin structure for each thread represented by class Processor.
 * 
 * A derived class must be capable of working in a multi-thread mode. The user should pay attention
 * to the fact that ROOT is not thread-safe. For this reason all critical blocks (which include, for
 * example, creation of any ROOT objects) must be guarded with the help of class ROOTLock.
 * 
 * A derived class must define a valid move constructor.
 * 
 * The user is not advised to inherit directly from this class but use classes ReaderPlugin and
 * AnalysisPlugin instead.
 */
class Plugin
{
public:
    /// Outcome of processing of an event
    enum class EventOutcome
    {
        Ok,            ///< Everything is fine
        FilterFailed,  ///< Event fails filtering implemented by the plugin
        NoEvents       ///< There are no events left in the input dataset
    };
    
public:
    /// Constructor
    Plugin(std::string const &name);
    
    /// Default copy constructor
    Plugin(Plugin const &) = default;
    
    /// Default move constructor
    Plugin(Plugin &&) = default;
    
    /// Default assignment operator
    Plugin &operator=(Plugin const &) = default;
    
    /// Trivial destructor
    virtual ~Plugin();

public:
    /**
     * \brief Provides a pointer to an instance of Processor class that owns the plugin
     * 
     * The pointer is guaranteed to be initialized before the first call to BeginRun. It stays
     * valid for the lifetime of the object.
     */
    void SetMaster(Processor const *processor);
    
    /// Returns name of the plugin
    std::string const &GetName() const;
    
    /**
     * \brief Clones the object
     * 
     * The method must create a new instance of the (derived) class with the same constructor
     * parameters. The method must not address any parameters specific to a run or an event.
     * Technically it means the method must create a new instance of the class exactly in the
     * same way this has been created and initialized.
     * 
     * The method is used when unique copies of plugins are created for each instance of class
     * Processor. Clonning is performed before call to SetMaster and before the first call to
     * BeginRun.
     */
    virtual Plugin *Clone() const = 0;
    
    /**
     * \brief Performs initialization needed when processing of a new dataset starts
     * 
     * The method is trivial in the default implementation.
     */
    virtual void BeginRun(Dataset const &dataset);
    
    /**
     * \brief Performs necessary actions needed after processing of a dataset is finished
     * 
     * The method is trivial in the default implementation.
     */
    virtual void EndRun();
    
    /**
     * \brief Processes a new event from the current dataset
     * 
     * The actual processing is delegated to pure virtual method ProcessEvent, which is to be
     * implemented in a derived class. The boolean decision of this method is reinterpreted with the
     * help of pure virtual method ReinterpretDecision.
     * 
     * The returned value can used by the parent Processor to alter execution of the event loop.
     */
    EventOutcome ProcessEventToOutcome();
    
private:
    /**
     * \brief A hook to implement actual processing of a new event from the current dataset
     * 
     * Interpretation of the returned boolean value depends on the type of the plugin and performed
     * by the pure virtual method ReinterpretDecision.
     */
    virtual bool ProcessEvent() = 0;
    
    /**
     * \brief Converts boolean decision issued by ProcessEvent to type EventOutcome
     * 
     * The specific conversion rule depends on the type of the plugin and must be implemented in a
     * derived class.
     */
    virtual EventOutcome ReinterpretDecision(bool decision) const = 0;
    
protected:
    /// Unique name to identify the plugin
    std::string const name;
    
    /// Processor object that owns the plugin
    Processor const *master;
};

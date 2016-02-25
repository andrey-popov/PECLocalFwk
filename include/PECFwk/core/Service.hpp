#pragma once

#include <PECFwk/core/ProcessorForward.hpp>
#include <PECFwk/core/DatasetForward.hpp>

#include <string>


/**
 * \class Service
 * \brief Abstract base class for services
 * 
 * This class provides an interface to access information that is only updated once per dataset or
 * not updated at all (as opposed to plugins that are designed to operate on the per-event basis).
 * It keeps the pointer to the Processor class that owns services, which allows to access other
 * services if needed.
 * 
 * A service is notified when processing of a dataset starts or finishes with the help of the
 * dedicated hooks. Depending on the logic, it might or might not take action in response to these
 * events.
 * 
 * A service must implement method Clone that creates a newly initialized copy of it. The clonning
 * is performed by the framework before the start of processing of the first dataset, and thus it
 * must not address any internal state specific to a dataset. Often it might be advantageous to
 * share resources between all clones of a service.
 * 
 * A service must be capable of working in a multi-thread environment. In particular, a special
 * attention should be given to ROOT objects as ROOT is not thread-safe. Critical blocks must be
 * guarded with the help of class ROOTLock.
 * 
 * A service must define a valid move constructor.
 */
class Service
{
public:
    /**
     * \brief Constructor
     * 
     * The argument is the unique name of the service.
     */
    Service(std::string const &name);
    
    /// Default copy constructor
    Service(Service const &) = default;
    
    /// Default move constructor
    Service(Service &&) = default;
    
    /// Default assignment operator
    Service &operator=(Service const &) = default;
    
    /// Trivial destructor
    virtual ~Service();
    
public:
    /**
     * \brief Performs initialization needed when processing of a new dataset starts
     * 
     * The method is trivial in the default implementation.
     */
    virtual void BeginRun(Dataset const &dataset);
    
    /**
     * \brief Clones the object
     * 
     * The method must create a new instance of the (derived) class with the same constructor
     * parameters. The method must not address any parameters specific to a dataset. Technically
     * it means the method must create a new instance of the class exactly in the same way this has
     * been created and initialized.
     * 
     * In some cases, it might be advantageous to share resources between all clones, provided that
     * precautions against race conditions are taken.
     * 
     * The method is used when unique copies of services are created for each instance of class
     * Processor. Clonning is performed before call to SetMaster and before the first call to
     * BeginRun.
     */
    virtual Service *Clone() const = 0;
    
    /**
     * \brief Performs necessary actions needed after processing of a dataset is finished
     * 
     * The method is trivial in the default implementation.
     */
    virtual void EndRun();
    
    /**
     * \brief Returns a reference to the master
     * 
     * Will throw an exception if the pointer to master is null.
     */
    Processor const &GetMaster() const;
    
    /// Returns name of the plugin
    std::string const &GetName() const;
    
    /**
     * \brief Provides a pointer to an instance of Processor class that owns the service
     * 
     * The pointer is guaranteed to be initialized before the first call to BeginRun. It stays
     * valid for the lifetime of the object.
     */
    void SetMaster(Processor const *processor);
        
private:
    /// Unique name of the service
    std::string const name;
    
    /// Processor object that owns the service
    Processor const *master;
};

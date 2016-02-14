/**
 * \file PECReaderPlugin.hpp
 * 
 * The module defines a wrapper of PECReader class into a plugin.
 */

#pragma once

#include <PECFwk/core/ReaderPlugin.hpp>
#include <PECFwk/core/PECReader.hpp>
#include <PECFwk/core/PECReaderConfig.hpp>

#include <memory>


/**
 * \class RECReaderPlugin
 * \brief Wraps PECReader class into a plugin
 * 
 * The class wraps up an instance of class PECReader into a plugin. For each dataset read a new
 * instance of PECReader is constructed; however, the same PECReader configuration is recycled for
 * for all datasets. The configuration is owned by the plugin.
 * 
 * Consult documentation for the base class for a description of the interface.
 */
class PECReaderPlugin: public ReaderPlugin
{
public:
    /**
     * \brief Constructor
     * 
     * Given configuration is deeply copied by the constructor, and the object owns the copy.
     */
    PECReaderPlugin(std::unique_ptr<PECReaderConfig> &&config);
    
    /// Move constructor
    PECReaderPlugin(PECReaderPlugin &&src);
    
    /// Copy constructor is deleted
    PECReaderPlugin(PECReaderPlugin const &) = delete;
    
    /// Destructor
    ~PECReaderPlugin();

public:
    /**
     * \brief Implementation of cloning
     * 
     * Consult documentation of the base class. Pay attention to the meaning of this method is
     * slightly different from the usual one.
     */
    Plugin *Clone() const;
    
    /**
     * \brief Called before processing of a new dataset is started
     * 
     * Consult documentation of the base class.
     */
    void BeginRun(Dataset const &dataset);
    
    /**
     * \brief Called after processing of a dataset is finished
     * 
     * Consult documentation of the base class.
     */
    void EndRun();
    
    /**
     * \brief Returns a reference to the underlying PECReader object
     * 
     * If no valied object is associated to the plugin, an exception is thrown.
     */
    PECReader const &operator*() const;
    
    /// Returns a pointer to the underlying PECReader object
    PECReader const *operator->() const;
    
private:
    /**
     * \brief Called for each event
     * 
     * Consult documentation of the base class for a general objective of the method. In this
     * derived class the method returns false if only there are no more events in the dataset.
     */
    bool ProcessEvent();
    
private:
    /// A pointer to a current instance of class PECReader
    PECReader *reader;
    
    /**
     * \brief Configuration for an instance of class PECReader
     * 
     * The configuration is owned by the plugin. It is used to construct instances of class
     * PECReader for each dataset.
     */
    std::unique_ptr<PECReaderConfig> readerConfig;
};

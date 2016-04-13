#pragma once

#include <mensura/core/ReaderPlugin.hpp>

#include <mensura/core/PhysicsObjects.hpp>

#include <vector>


/**
 * \class GenJetMETReader
 * \brief An abstract base class for a reader plugin that provides collection generator-level jets
 * and MET
 */
class GenJetMETReader: public ReaderPlugin
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    GenJetMETReader(std::string const name = "GenJetMET");
    
    /// Default copy constructor
    GenJetMETReader(GenJetMETReader const &) = default;
    
    /// Default move constructor
    GenJetMETReader(GenJetMETReader &&) = default;
    
    /// Default assignment operator
    GenJetMETReader &operator=(GenJetMETReader const &) = default;
    
    /// Trivial destructor
    virtual ~GenJetMETReader() noexcept;
    
public:
    /// Returns collection of corrected jets in the current event
    std::vector<GenJet> const &GetJets() const;
    
    /// Returns radius parameter used in the jet clustering algorithm
    virtual double GetJetRadius() const = 0;
    
    /// Returns corrected MET in the current event
    MET const &GetMET() const;
    
protected:
    /// Collection of generator-level jets in the current event
    std::vector<GenJet> jets;
    
    /// Corrected MET in the current event
    MET met;
};

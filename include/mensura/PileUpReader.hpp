#pragma once

#include <mensura/ReaderPlugin.hpp>


/**
 * \class PileUpReader
 * \brief Abstract base class for a reader plugin that provides information about pile-up
 */
class PileUpReader: public ReaderPlugin
{
public:
    /**
     * \brief Creates plugin with the given name
     * 
     * User is encouraged to keep the default name.
     */
    PileUpReader(std::string const name = "PileUp");
    
    /// Default copy constructor
    PileUpReader(PileUpReader const &) = default;
    
    /// Default move constructor
    PileUpReader(PileUpReader &&) = default;
    
    /// Assignment operator is deleted
    PileUpReader &operator=(PileUpReader const &) = delete;
    
    /// Trivial destructor
    virtual ~PileUpReader();
    
public:
    /// Returns number of reconstructed primary vertices
    unsigned GetNumVertices() const;
    
    /**
     * \brief Returns expected amount of pile-up (aka "true" number of interactions)
     * 
     * In data zero is returned.
     */
    double GetExpectedPileUp() const;
    
    /// Returns median angular pt density (GeV)
    double GetRho() const;
    
protected:
    /// Number of reconstructed primary vertices
    unsigned numVertices;
    
    /// Expected pile-up
    double expectedPileUp;
    
    /// Median angular pt density, GeV
    double rho;
};

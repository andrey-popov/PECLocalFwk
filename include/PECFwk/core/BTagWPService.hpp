#pragma once 

#include <PECFwk/core/Service.hpp>

#include <PECFwk/core/BTagger.hpp>
#include <PECFwk/core/PhysicsObjects.hpp>

#include <string>
#include <unordered_map>


/**
 * \class BTagWPService
 * \brief A class to perform b-tagging
 * 
 * The class provides methods to check if a jet is b-tagged. Although the check can be performed
 * for any supported working point, short-cuts that use a default working point are provided. In
 * addition, the class declares enumerations to specify b-tagging algorithm and working point; they
 * are expected to be used in other classes related to b-tagging.
 * 
 * It is recommended that b-tagging is performed by the means of this class only but never using
 * values of b-tagging discriminators provided by class Jet.
 * 
 * The class provides valid copy and move constructors and assignment operator. Is is thread-safe.
 */
class BTagWPService: public Service
{
public:
    /**
     * \brief Constructor
     * 
     * The arguments specify the desired b-tagging algorithm and default working point
     */
    BTagWPService(std::string name = "BTagWPService");
    
    /// Default copy constructor
    BTagWPService(BTagWPService const &) = default;
    
    /// Default move constructor
    BTagWPService(BTagWPService &&) = default;
    
    /// Assignment operator is deleted
    BTagWPService &operator=(BTagWPService const &) = delete;
    
    /// Trivial destructor
    virtual ~BTagWPService() noexcept;

public:
    /**
     * \brief Checks if a jet is b-tagged according to the given tagger
     * 
     * Returns false if the jet is outside a pseudorapidity acceptance defined according to
     * BTagSFInterface::GetMaxPseudorapidity(). If the requested working point is not supported, an
     * exception is thrown.
     */
    bool IsTagged(BTagger const &tagger, Jet const &jet) const;
    
    /// Sets or changes numeric threshold for the given b tagger
    void SetThreshold(BTagger const &tagger, double threshold);
    
private:
    /// Numerical thresholds to define b-tagged jets
    std::unordered_map<BTagger, double> thresholds;
};

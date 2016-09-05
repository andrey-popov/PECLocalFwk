#pragma once 

#include <mensura/core/Service.hpp>

#include <mensura/core/BTagger.hpp>
#include <mensura/core/PhysicsObjects.hpp>

#include <string>
#include <unordered_map>


/**
 * \class BTagWPService
 * \brief A class to perform b-tagging
 * 
 * This class maintains b-tagging thresholds and provides a method to check if a jet is b-tagged
 * according to the given tagger. The thresholds are normally read from a JSON file, but they can
 * also be set or overwritten manually with the help of method SetThreshold.
 * 
 * It is recommended that b-tagging is performed by the means of this class only but never using
 * values of b-tagging discriminators provided by class Jet.
 * 
 * The class provides valid copy and move constructors. Is is thread-safe.
 */
class BTagWPService: public Service
{
public:
    /**
     * \brief Creates a new service with the given name and reads thresholds from the given file
     * 
     * The path to the JSON file with thresholds is resolved with the FileInPath servce, using a
     * default subdirectory "BTag". User may also provide an empty file name. In this case
     * constructor does not set any thresholds, which should instead be provided manually using
     * method SetThreshold.
     */
    BTagWPService(std::string const &name, std::string const &dataFileName);
    
    /// Copy of the above with the default name "BTagWP"
    BTagWPService(std::string const &dataFile);
    
    /// Default copy constructor
    BTagWPService(BTagWPService const &) = default;
    
    /// Default move constructor
    BTagWPService(BTagWPService &&) = default;
    
    /// Assignment operator is deleted
    BTagWPService &operator=(BTagWPService const &) = delete;

public:
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Service.
     */
    virtual Service *Clone() const override;
    
    /// Returns numeric threshold for the given b tagger
    double GetThreshold(BTagger const &tagger) const;
    
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

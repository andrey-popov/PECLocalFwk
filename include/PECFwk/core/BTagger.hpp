/**
 * \file BTagger.hpp
 * 
 * The module provides means to perform b-tagging.
 */

#pragma once 

#include <PECFwk/core/PhysicsObjects.hpp>

#include <string>
#include <map>


/**
 * \class BTagger
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
class BTagger
{
public:
    /// Supported b-tagging algorithms
    enum class Algorithm
    {
        CSV,     ///< Combined secondary vertex
        JP,      ///< Jet probability
        TCHP,    ///< Track counting high purity
        CSVV1,   ///< Retrained CSV
        CSVSLV1  ///< A version of CSV with additional information on soft leptons exploited
    };
    
    /// Supported working points for the b-tagging algorithms
    enum class WorkingPoint
    {
        Tight,
        Medium,
        Loose
    };

public:
    /**
     * \brief Constructor
     * 
     * The arguments specify the desired b-tagging algorithm and default working point
     */
    BTagger(Algorithm algo, WorkingPoint defaultWP = WorkingPoint::Tight);
    
    /// Copy constructor
    BTagger(BTagger const &src);
    
    /// Move constructor
    BTagger(BTagger &&src);
    
    /// Assignment operator
    BTagger &operator=(BTagger const &rhs);

public:
    /**
     * \brief Checks if a jet is b-tagged according to the given working point
     * 
     * Returns false if the jet is outside a pseudorapidity acceptance defined according to
     * BTagSFInterface::GetMaxPseudorapidity(). If the requested working point is not supported, an
     * exception is thrown.
     */
    bool IsTagged(WorkingPoint wp, Jet const &jet) const;
    
    /**
     * \brief Checks if a jet is b-tagged according to the default working point
     * 
     * Internally calls IsTagged(WorkingPoint, Jet const &).
     */
    bool IsTagged(Jet const &jet) const;
    
    /**
     * \brief A short-cut for IsTagged method
     * 
     * Internally calls IsTagged(WorkingPoint, Jet const &).
     */
    bool operator()(WorkingPoint wp, Jet const &jet) const;
    
    /**
     * \brief A short-cut for IsTagged method
     * 
     * Internally calls IsTagged(WorkingPoint, Jet const &).
     */
    bool operator()(Jet const &jet) const;
    
    /// Returns the b-tagging algorithm in use
    Algorithm GetAlgorithm() const;
    
    /// Returns the default working point
    WorkingPoint GetDefaultWorkingPoint() const;
    
    /// (Depricated. Do not use this method)
    WorkingPoint GetWorkingPoint() const;
    
    /// Returns a string that encodes the algorithm and the working point
    std::string GetTextCode() const;
    
private:
    /// Chosen b-tagging algorithm
    Algorithm algo;
    
    /// Default working point of the b-tagging algorithm
    WorkingPoint defaultWP;
    
    /// Numerical thresholds for the chosen b-tagging algorithm
    std::map<WorkingPoint, double> thresholds;
    
    /// Pointer to a method of class Jet to access an appropriate b-tagging discriminator
    double (Jet::*bTagMethod)() const;
};

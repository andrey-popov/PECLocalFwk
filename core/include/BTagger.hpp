/**
 * \file BTagger.hpp
 * \author Andrey Popov
 * 
 * The module provides means to perform b-tagging.
 */

#pragma once 

#include <PhysicsObjects.hpp>

#include <string>


/**
 * \class BTagger
 * \brief A class to perform b-tagging
 * 
 * The class provides a method to check if a jet is b-tagged. It also declares enumerations to
 * specify the algorithm and the working point, which can be used in other classes to include
 * b-tagging scale factors.
 * 
 * Note that b-tagging must only be performed with the help of this class.
 * 
 * After an object is constructed, class data members are only read. Therefore, the class is thread-
 * safe.
 */
class BTagger
{
    public:
        /// Supported b-tagging algorithms
        enum class Algorithm
        {
            CSV,   ///< Combined secondary vertex
            JP,    ///< Jet probability
            TCHP   ///< Track counting high purity
        };
        
        /// Supported working points for the b-tagging algorithms
        enum class WorkingPoint
        {
            Tight,
            Medium,
            Loose
        };
    
    public:
        /// Constructor
        BTagger(Algorithm algo_, WorkingPoint workingPoint_);
        
        /// Default copy constructor
        BTagger(BTagger const &) = default;
    
    public:
        /// Checks if a jet is b-tagged
        bool IsTagged(Jet const &jet) const;
        
        /// A short-cut for IsTagged method
        bool operator()(Jet const &jet) const;
        
        /// Returns the b-tagging algorithm in use
        Algorithm GetAlgorithm() const;
        
        /// Returns the working point in use
        WorkingPoint GetWorkingPoint() const;
        
        /// Returns a string that encodes the algorithm and the working point
        std::string GetTextCode() const;
        
    private:
        Algorithm algo;  ///< The b-tagging algorithm
        WorkingPoint workingPoint;  ///< The working point of the b-tagging algorithm
        double threshold;  ///< The numerical threshold for the b-tagging discriminator
        double (Jet::*bTagMethod)() const;  ///< Pointer to the method to access the discriminator
};
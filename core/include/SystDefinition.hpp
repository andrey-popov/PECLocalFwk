/**
 * \file SystDefinition.hpp
 * \author Andrey Popov
 * 
 * The module defines auxiliary data types to choose specific sources of systematical uncertainties.
 * Algorithms to propagate the uncertainties are not considered here and are decribed in dedicated
 * modules.
 */

#pragma once


/**
 * \enum SystTypeAlgo
 * \brief Types of systematical variations that can be requested from class PECReader
 * 
 * The enumeration categorises sources of systematical uncertainties depending on how they are
 * accounted for in PECReader. Some sources affect event weights only and for this reason are
 * grouped into a single category. Other ones are assigned dedicated items of the enumaration. This
 * categorisation is used to request calculation of certain systematical variations within class
 * PECReader.
 * 
 * Direction of the variation is not specified by the enumeration.
 */
enum class SystTypeAlgo
{
    None,  ///< No systematical variation
    WeightOnly,  ///< All variations that are encoded in event weights
    JEC,  ///< Variation of jet energy correction
    JER,  ///< Variation of jet energy resolution
    METUnclustered  ///< Variation of unclustered MET
};


/**
 * \enum SystTypeWeight
 * \brief Supported types of systematical variations that affect event weight only
 * 
 * Sources of systematical uncertainties that affect event weights only. The enumeration is used
 * mostly to access varied event weights from an instance of class PECReader.
 * 
 * Direction of the variation is not specified by the enumeration and is never used for these
 * systematical uncertainties (the variations are always read altogether).
 */
enum class SystTypeWeight
{
    PileUp,  ///< Variation in amount of pile-up
    TagRate,  ///< Variation of b-tagging scale factors for b- and c-jets
    MistagRate,  ///< Variation of b-tagging scale factors for light-flavour jets
    PDF  ///< Variation in parton distribution functions (not implemented yet)
};


/**
 * \struct SystVariation
 * \brief Aggregates a type of systematical variation and its direction
 * 
 * The primary use-case for this class is to instruct an instance of class PECReader to perform a
 * specific systematical variation.
 */
struct SystVariation
{
    /// Constructor without parameters
    SystVariation();
    
    /// Constructor with a simple initialisation
    SystVariation(SystTypeAlgo type, int direction);
    
    /// Default copy constructor
    SystVariation(SystVariation const &) = default;
    
    /// Default assignment operator
    SystVariation &operator=(SystVariation const &) = default;
    
    /// Resets the data members
    void Set(SystTypeAlgo type, int direction);
    
    /// Type of systematical uncertainty
    SystTypeAlgo type;
    
    /// Direction of systematical variation (+1, -1, or 0 if switched off)
    int direction;
};


/**
 * \struct WeightPair
 * \brief Aggregates event weights for up and down variations for one independent source of
 * systematical uncertainty
 */
struct WeightPair
{
    /// Constructor without parameters
    WeightPair();
    
    /// Constructor with a simple initialisation
    WeightPair(double up, double down);
    
    /// Default copy constructor
    WeightPair(WeightPair const &) = default;
    
    /// Default assignment operator
    WeightPair &operator=(WeightPair const &) = default;
    
    /// Resets data members
    void Set(double up, double down);
    
    /// "Up" variation
    double up;
    
    /// "Down" variation
    double down;
};

#pragma once 

#include <functional>
#include <string>


/**
 * \class BTagger
 * \brief A light-weight class to define selection on b-tagging
 * 
 * The class stores selected b-tagging algorithm and working point. It is strongly recommended to
 * defined b-tagged jets with the help of this class instead of explicit cuts on values of
 * b-tagging disriminators.
 */
class BTagger
{
public:
    /// Supported b-tagging algorithms
    enum class Algorithm: unsigned
    {
        CSV,     ///< Combined secondary vertex, v2
        JP,      ///< Jet probability
        CMVA,    ///< Combined MVA, v2
        DeepCSV  ///< CSV with DeepNN
    };
    
    /// Supported working points for b-tagging algorithms
    enum class WorkingPoint
    {
        Tight,
        Medium,
        Loose
    };
    
public:
    /// Constructor with full initialization
    BTagger(Algorithm algo, WorkingPoint wp);
    
    /// Default copy constructor
    BTagger(BTagger const &) = default;
    
    /// Default move constructor
    BTagger(BTagger &&) = default;
    
    /// Default assignment operator
    BTagger &operator=(BTagger const &) = default;
    
public:
    /// Converts algorithm ID to a text code
    static std::string AlgorithmToTextCode(Algorithm algo);
    
    /// Returns the b-tagging algorithm in use
    Algorithm GetAlgorithm() const;
    
    /**
     * \brief Returns maximal jet pseudorapidity for which b-tagging is supported
     * 
     * A very small fraction of jets with larger pseudorapidity can be b-tagged, but they should
     * not be considered as such in an analysis. One reasons is that scale factors are always
     * constrained to |eta| < 2.4.
     */
    static constexpr double GetMaxPseudorapidity();
    
    /// Returns a string that encodes the algorithm and the working point
    std::string GetTextCode() const;
    
    /// Returns the working point in use
    WorkingPoint GetWorkingPoint() const;
    
    /// Hash function to use BTagger in an unordered map
    std::size_t Hash() const;
    
    /// Equality operator
    bool operator==(BTagger const &other) const;
    
    /// Converts working point ID to a text code
    static std::string WorkingPointToTextCode(WorkingPoint wp);
    
private:
    /// Selected b-tagging algorithm
    Algorithm algo;
    
    /// Selected working point
    WorkingPoint wp;
    
    /**
     * \brief Number of supported working points
     * 
     * This constant is needed to calculate hash value of the object.
     */
    static unsigned const numWP = 3;
};


// Must define constexpr functions in the header since they are implicitly inlined
constexpr double BTagger::GetMaxPseudorapidity()
{
    return 2.4;
}



namespace std
{
/**
 * \struct std::hash<BTagger>
 * \brief Specialization of std::hash for the b-tagger
 */
template<>
struct hash<BTagger>
{
    std::size_t operator()(BTagger const &tagger) const
    {
        return tagger.Hash();
    }
};
};

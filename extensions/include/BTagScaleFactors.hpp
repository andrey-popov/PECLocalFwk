/**
 * \file BTagScaleFactors.hpp
 * \author Andrey Popov
 * 
 * Defines a class to access b-tagging scale factors.
 */

#pragma once

#include <BTagSFInterface.hpp>

#include <map>


/**
 * \class BTagScaleFactors
 * \brief Retrieves b-tagging scale factors
 * 
 * The format in which scale factors are provided makes it difficult to store them in an
 * interchangeable file. For this reason the scale factors are hard-coded. A fraction of the code
 * was generated automatically by the script [1]. The scale factors recommended for EPS'13 [2] are
 * provided.
 * [1] /afs/cern.ch/user/a/aapopov/workspace/tHq/2012Bravo/2014.02.17_BTagSF
 * [2] https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagPOG?rev=182#2012_Data_and_MC_EPS13_prescript
 * 
 * The class defines valid copy and move constructors. It is thread-safe.
 * 
 * Consult documentation on the base class for additional information.
 */
class BTagScaleFactors: public BTagSFInterface
{
private:
    /// Pointers to methods to access scale factors for one working point
    struct RawSFSingleWP
    {
        double (*tagSF)(double);
        double (*tagSFUnc)(double);
        double (*mistagSFMean)(double, double);
        double (*mistagSFMin)(double, double);
        double (*mistagSFMax)(double, double);
    };
    
public:
    /**
     * \brief Constructor
     * 
     * A b-tagging algorithm should be provided as an input. If the algorithm is not supported, an
     * exception is thrown.
     */
    BTagScaleFactors(BTagger::Algorithm algo);
    
    /// Copy constructor
    BTagScaleFactors(BTagScaleFactors const &src) noexcept;
    
    /// Move constructor
    BTagScaleFactors(BTagScaleFactors &&src) noexcept;
    
    /// Assignment operator
    BTagScaleFactors &operator=(BTagScaleFactors const &rhs) noexcept;
    
    /// Trivial virtual destructor
    virtual ~BTagScaleFactors() noexcept;
    
public:
    /// Returns a newly allocated copy of *this created with the copy constructor
    virtual BTagSFInterface *Clone() const;
    
    /**
     * \brief Returns b-tagging scale factor for a given working point and a given jet
     * 
     * The method constrains jet transverse momentum to the appropriate range described in [1] and
     * calls specific automatically generated methods to obtain the scale factor. If the scale
     * factor is not available for the given working point, an exception is thrown. If the method
     * is called for a forward jet (defined according to BTagSFInterface::GetMaxPseudorapidity),
     * an exception is thrown as well.
     * [1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagPOG
     */
    virtual double GetScaleFactor(BTagger::WorkingPoint wp, Jet const &jet,
     Variation var = Variation::Nominal) const;
    
private:
    // The code below was generated automatically
    static double GetSFTagTCHPT(double pt);
    static double GetSFTagCSVL(double pt);
    static double GetSFTagCSVM(double pt);
    static double GetSFTagCSVT(double pt);
    static double GetSFTagCSVV1L(double pt);
    static double GetSFTagCSVV1M(double pt);
    static double GetSFTagCSVV1T(double pt);
    static double GetSFTagCSVSLV1L(double pt);
    static double GetSFTagCSVSLV1M(double pt);
    static double GetSFTagCSVSLV1T(double pt);
    
    static double GetSFTagUncTCHPT(double pt);
    static double GetSFTagUncCSVL(double pt);
    static double GetSFTagUncCSVM(double pt);
    static double GetSFTagUncCSVT(double pt);
    static double GetSFTagUncCSVV1L(double pt);
    static double GetSFTagUncCSVV1M(double pt);
    static double GetSFTagUncCSVV1T(double pt);
    static double GetSFTagUncCSVSLV1L(double pt);
    static double GetSFTagUncCSVSLV1M(double pt);
    static double GetSFTagUncCSVSLV1T(double pt);
    
    static double CalcSFTagUnc(double pt, double const *uncBins);
    
    static double GetSFMistagTCHPTMean(double pt, double absEta);
    static double GetSFMistagTCHPTMin(double pt, double absEta);
    static double GetSFMistagTCHPTMax(double pt, double absEta);
    static double GetSFMistagCSVLMean(double pt, double absEta);
    static double GetSFMistagCSVLMin(double pt, double absEta);
    static double GetSFMistagCSVLMax(double pt, double absEta);
    static double GetSFMistagCSVMMean(double pt, double absEta);
    static double GetSFMistagCSVMMin(double pt, double absEta);
    static double GetSFMistagCSVMMax(double pt, double absEta);
    static double GetSFMistagCSVTMean(double pt, double absEta);
    static double GetSFMistagCSVTMin(double pt, double absEta);
    static double GetSFMistagCSVTMax(double pt, double absEta);
    static double GetSFMistagCSVV1LMean(double pt, double absEta);
    static double GetSFMistagCSVV1LMin(double pt, double absEta);
    static double GetSFMistagCSVV1LMax(double pt, double absEta);
    static double GetSFMistagCSVV1MMean(double pt, double absEta);
    static double GetSFMistagCSVV1MMin(double pt, double absEta);
    static double GetSFMistagCSVV1MMax(double pt, double absEta);
    static double GetSFMistagCSVV1TMean(double pt, double absEta);
    static double GetSFMistagCSVV1TMin(double pt, double absEta);
    static double GetSFMistagCSVV1TMax(double pt, double absEta);
    static double GetSFMistagCSVSLV1LMean(double pt, double absEta);
    static double GetSFMistagCSVSLV1LMin(double pt, double absEta);
    static double GetSFMistagCSVSLV1LMax(double pt, double absEta);
    static double GetSFMistagCSVSLV1MMean(double pt, double absEta);
    static double GetSFMistagCSVSLV1MMin(double pt, double absEta);
    static double GetSFMistagCSVSLV1MMax(double pt, double absEta);
    static double GetSFMistagCSVSLV1TMean(double pt, double absEta);
    static double GetSFMistagCSVSLV1TMin(double pt, double absEta);
    static double GetSFMistagCSVSLV1TMax(double pt, double absEta);
    // End of automatically generated code
    
private:
    /**
     * \brief Association between working point and a set of methods to calculate scale factors for
     * the selected algorithm
     */
    std::map<BTagger::WorkingPoint, RawSFSingleWP> rawScaleFactors;
    
    /**
     * \brief Maximal pt to evaluate scale factors for b- or c-quark jets
     * 
     * This value depends on the b-tagging algorithm and must be adjusted in the constructor.
     */
    double ptMaxTag;
};

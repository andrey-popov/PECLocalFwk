/**
 * \file JetResolutionFactor.hpp
 * \author Andrey Popov
 * 
 * Defines a class to perform JER smearing in simulation.
 */

#pragma once

#include <PhysicsObjects.hpp>

#include <TH2D.h>

#include <string>
#include <memory>


/**
 * \class JetResolutionFactor
 * \brief A class to perform JER smearing
 * 
 * The JER smearing is performed according to the deterministic method in [1]. Its implementation
 * reproduces one in [2]. However, only jets with a generator-level match are smeared. The class
 * relies on the default matching performed in PECReader (which most likely follows the definition
 * from JME-13-005), and it might differ from [2].
 * [1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/JetResolution
 * [2] https://github.com/cms-sw/cmssw/blob/CMSSW_5_3_X/PhysicsTools/PatUtils/interface/SmearedJetProducerT.h
 */
class JetResolutionFactor
{
public:
    /**
     * \brief Supported systematical variations
     * 
     * The implementation relies on the numeric values.
     */
    enum class SystVar: int
    {
        Nominal = 0,
        Up = +1,
        Down = -1
    };
    
public:
    /**
     * \brief Constructor
     * 
     * The given file name is resolved with the help of FileInPath service.
     */
    JetResolutionFactor(std::string const &dataFileName) noexcept;
    
    /// Copy constructor
    JetResolutionFactor(JetResolutionFactor const &src) noexcept;
    
    /// Assignment operator is deleted
    JetResolutionFactor &operator=(JetResolutionFactor const &) = delete;
    
public:
    /**
     * \brief Computes a scale factor to account for JER smearing
     * 
     * The scale factor can be calculated if only given jet contains a generator-level match,
     * otherwise a unit scale factor is returned. The jet must be corrected for JEC.
     */
    double GetFactor(Jet const &jet, SystVar syst = SystVar::Nominal) const noexcept;
    
private:
    /// Name of the ROOT file containing a histogram with JER factors
    std::string dataFileName;
    
    /// The histogram with JER factors
    std::unique_ptr<TH2D> jerHist;
};

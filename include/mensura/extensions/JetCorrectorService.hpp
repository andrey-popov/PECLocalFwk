#pragma once

#include <mensura/core/Service.hpp>

#include <mensura/core/SystService.hpp>

#include <mensura/external/JERC/FactorizedJetCorrector.hpp>
#include <mensura/external/JERC/JetCorrectionUncertainty.hpp>
#include <mensura/external/JERC/JetResolution.hpp>

#include <TRandom3.h>

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>


class Jet;


/**
 * \class JetCorrectorService
 * \brief A service that computes jet energy and resolution corrections
 * 
 * This class computes jet corrections to adjust energy scale and resolution. Inputs for the
 * computation are provided in form of standard text files used by JetMET POG. The computed
 * correction is returned as a factor to rescale jet four-momentum.
 * 
 * Any levels of the full correction can be omitted. If JEC text files are not provided, jet
 * momentum is assumed to be corrected for the energy scale. Otherwise the corresponding correction
 * is evaluated starting from raw jet momentum (which thus must have been set up properly). If one
 * or more JEC uncertainties are specified by user and a JEC systematic variation is requested,
 * the correction factor is adjusted accordingly. If JER data/MC scale factors have been specified,
 * deterministic JER smearing is applied for jets that have matched generator-level jets. If pt
 * resolution in simulation is specified in addition, jets that do not have generator-level matches
 * are smeared stochastically using this resolution and the scale factors.
 */
class JetCorrectorService: public Service
{
public:
    /// Supported types of systematic variations
    enum class SystType
    {
        None,
        JEC,
        JER
    };
    
public:
    /// Creates a service with the given name
    JetCorrectorService(std::string const name = "JetCorrector");
    
    /// Copy constructor
    JetCorrectorService(JetCorrectorService const &src);
    
    /// Default move constructor
    JetCorrectorService(JetCorrectorService &&) = default;
    
public:
    /**
     * \brief Creates a newly-initialized clone
     * 
     * Implemented from Service.
     */
    virtual Service *Clone() const override;
    
    /**
     * \brief Computes full correction factor for requested effects
     * 
     * Returned factor can include correction of the energy scale and resolution. As explained in
     * documentation for the class, any part of the correction is optional.
     */
    double Eval(Jet const &jet, double rho, SystType syst = SystType::None,
      SystService::VarDirection direction = SystService::VarDirection::Undefined) const;
    
    /**
     * \brief Computes JEC uncertainty
     * 
     * The arguments are the JES-corrected pt and pseudorapidity. The uncertainty is calculated as
     * a sum in quadrature of uncertainties from all sources specified in a call to
     * SetJECUncertainty.
     */
    double EvalJECUnc(double const corrPt, double const eta) const;
    
    /// Reports if requested systematic variation can be computed
    bool IsSystEnabled(SystType syst) const;
    
    /// A short-cut for method Eval
    double operator()(Jet const &jet, double rho, SystType syst = SystType::None,
      SystService::VarDirection direction = SystService::VarDirection::Undefined) const;
    
    /**
     * \brief Specifies text files for JEC
     * 
     * Different levels of corrections are applied in the same order as specified here. Paths to
     * the files are resolved using FileInPath, with a subdirectory "JERC".
     */
    void SetJEC(std::initializer_list<std::string> const &jecFiles);
    
    /**
     * \brief Specifies text file for JEC uncertainties and desired uncertainty sources
     * 
     * There is no need to provide the name of an uncertainty source if the file defines only a
     * single one. Path to the file is resolved using FileInPath, with a subdirectory "JERC".
     */
    void SetJECUncertainty(std::string const &jecUncFile,
      std::initializer_list<std::string> uncSources = {});
    
    /**
     * Specifies text files for data/MC JER scale factors and pt resolution in MC
     * 
     * Paths to the files are resolved using FileInPath, with a subdirectory "JERC". The file with
     * pt resolution in simulation is optional. Only when it is specified, a random-number
     * generator is created to perform stochastic JER smearing. The last argument defines the seed
     * for the generator; 0 means that the seed will be chosen randomly.
     */
    void SetJER(std::string const &jerSFFile, std::string const &jerMCFile,
      unsigned long seed = 0);
    
private:
    /// Constructs an object to evaluate JEC
    void CreateJECEvaluator();
    
    /// Constructs an object to evaluate JEC uncertainty
    void CreateJECUncEvaluator();
    
    /// Constructs objects to evaluate effect of JER smearing
    void CreateJEREvaluator();
    
private:
    /**
     * \brief Files with requested jet energy corrections
     * 
     * Needed to create a clone of FactorizedJetCorrector. Paths to files are fully qualified.
     */
    std::vector<std::string> jecFiles;
    
    /**
     * \brief An object that evaluates jet energy corrections
     * 
     * Can be uninitialized if no JEC have been provided.
     */
    std::unique_ptr<FactorizedJetCorrector> jetEnergyCorrector;
    
    /**
     * \brief Path to file with requested JEC uncertainties
     * 
     * Needed to create a clone of JetCorrectionUncertainty. The path is fully qualified.
     */
    std::string jecUncFile;
    
    /**
     * \brief Requested sources of JEC uncertainty
     * 
     * Needed to create a clone of JetCorrectionUncertainty.
     */
    std::vector<std::string> jecUncSources;
    
    /**
     * \brief Objects that compute JEC uncertainties
     * 
     * The vector can be empty if no uncertainties have been specified.
     */
    std::vector<std::unique_ptr<JetCorrectionUncertainty>> jecUncProviders;
    
    /**
     * \brief Paths to files with JER scale factors and MC resolutions
     * 
     * Needed to create clones of JetResolution and JetResolutionScaleFactor. The paths are fully
     * qualified.
     */
    std::string jerSFFile, jerMCFile;
    
    /**
     * \brief An object that provides pt resolution in simulation
     * 
     * Can be uninitialized if resolutions have not been specified.
     */
    std::unique_ptr<JME::JetResolution> jerProvider;
    
    /**
     * \brief An object that provides data/MC scale factors for JER
     * 
     * Can be uninitialized if the scale factors have not been specified.
     */
    std::unique_ptr<JME::JetResolutionScaleFactor> jerSFProvider;
    
    /**
     * \brief Random-number generator
     * 
     * Used for stochastic JER smearing. Only created when pt resolution in simulation is
     * specified.
     */
    std::unique_ptr<TRandom3> rGen;
};

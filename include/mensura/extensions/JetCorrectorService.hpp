#pragma once

#include <mensura/core/Service.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/core/SystService.hpp>

#include <mensura/external/JERC/FactorizedJetCorrector.hpp>
#include <mensura/external/JERC/JetCorrectionUncertainty.hpp>
#include <mensura/external/JERC/JetResolution.hpp>

#include <TRandom3.h>

#include <initializer_list>
#include <map>
#include <memory>
#include <string>
#include <type_traits>
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
 * Intervals of validity (IOVs) in terms of run ranges are supported. User must register all IOVs
 * with method RegisterIOV and provide JERC parameters for each of them using methods
 * SetJEC, SetJECUncertainty, and SetJER. Before jet corrections can be evaluated, the appropriate
 * IOV must be chosen with SelectIOV.
 * 
 * Alternatively, it is possible to use a single match-all IOV. It does not need to be registered,
 * and there are dedicated versions of methods to provide JERC parameters. There is also no need to
 * call for SelectIOV in that case.
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
    
private:
    /// Auxiliary structure to aggregate JERC parameters for a single IOV
    struct IOVParams
    {
        /// Constructor from a run range
        IOVParams(unsigned long minRun, unsigned long maxRun);
        
        /**
         * \brief Run range for this IOV
         * 
         * The boundaries are included in the range.
         */
        unsigned long minRun, maxRun;
        
        /**
         * \brief Files with requested jet energy corrections
         * 
         * Paths to files are fully qualified.
         */
        std::vector<std::string> jecFiles;
        
        /**
         * \brief Path to file with requested JEC uncertainties
         * 
         * The path is fully qualified.
         */
        std::string jecUncFile;
        
        /// Requested sources of JEC uncertainty
        std::vector<std::string> jecUncSources;
        
        /**
         * \brief Paths to files with JER scale factors and MC resolutions
         * 
         * The paths are fully qualified.
         */
        std::string jerSFFile, jerMCFile;
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
     * \brief Computes full correction factor for requested effects with the current IOV
     * 
     * Returned factor can include correction of the energy scale and resolution. As explained in
     * documentation for the class, any part of the correction is optional.
     */
    double Eval(Jet const &jet, double rho, SystType syst = SystType::None,
      SystService::VarDirection direction = SystService::VarDirection::Undefined) const;
    
    /**
     * \brief Computes JEC uncertainty with the current IOV
     * 
     * The arguments are the JES-corrected pt and pseudorapidity. The uncertainty is calculated as
     * a sum in quadrature of uncertainties from all sources specified in a call to
     * SetJECUncertainty.
     */
    double EvalJECUnc(double const corrPt, double const eta) const;
    
    /// Reports if requested systematic variation can be computed with the current IOV
    bool IsSystEnabled(SystType syst) const;
    
    /// A short-cut for method Eval
    double operator()(Jet const &jet, double rho, SystType syst = SystType::None,
      SystService::VarDirection direction = SystService::VarDirection::Undefined) const;
    
    /**
     * \brief Registers a new IOV
     * 
     * The IOV is defined with the given run range (the boundaries are included in the range). The
     * first argument specifies an arbitrary label to identify this IOV for the purpose of
     * configuration.
     */
    void RegisterIOV(std::string const &label, unsigned long minRun, unsigned long maxRun);
    
    /// Selects IOV that includes the given run
    void SelectIOV(unsigned long run) const;
    
    /**
     * \brief Specifies text files for JEC
     * 
     * Different levels of corrections are applied in the same order as specified here. Paths to
     * the files are resolved using FileInPath, with a subdirectory "JERC".
     */
    template<typename C = std::initializer_list<std::string>>
    void SetJEC(std::string const &iovLabel, C const &jecFiles);
    
    /**
     * \brief Specifies text files for JEC for the match-all implicit IOV
     * 
     * This is a special version of the above method to be used when no explicit IOVs are defined.
     */
    template<typename C = std::initializer_list<std::string>>
    void SetJEC(C const &jecFiles);
    
    /**
     * \brief Specifies text file for JEC uncertainties and desired uncertainty sources
     * 
     * Path to the file is resolved using FileInPath, with a subdirectory "JERC". If the file
     * defines only a single uncertainty source, the last argument may be an empty collection.
     */
    template<typename C = std::initializer_list<std::string>>
    void SetJECUncertainty(std::string const &iovLabel, std::string const &jecUncFile,
      C const &uncSources);
    
    /**
     * \brief Specifies text file for JEC uncertainties and desired uncertainty sources
     * 
     * Specialized version of SetJECUncertainty for a single uncertainty source. If the file
     * defines only a single source, its label may be omitted.
     */
    void SetJECUncertainty(std::string const &iovLabel, std::string const &jecUncFile,
      std::string uncSource = "");
    
    /**
     * \brief Specifies text files for JEC uncertainties for the match-all implicit IOV
     * 
     * Specialized version of SetJECUncertainty to be used when no explicit IOVs are defined.
     */
    template<typename C = std::initializer_list<std::string>>
    void SetJECUncertainty(std::string const &jecUncFile, C const &uncSources);
    
    /**
     * \brief Specifies text file for JEC uncertainties and desired uncertainty sources
     * 
     * Specialized version of SetJECUncertainty for the case when no explicit IOVs are defined and
     * a single uncertainty source is used. If the file defines only a single uncertainty source,
     * its label may be omitted.
     */
    void SetJECUncertainty(std::string const &jecUncFile, std::string uncSource = "");
    
    /**
     * Specifies text files for data/MC JER scale factors and pt resolution in MC
     * 
     * Paths to the files are resolved using FileInPath, with a subdirectory "JERC". The file with
     * pt resolution in simulation is optional. Only when it is specified, a random-number
     * generator is created to perform stochastic JER smearing.
     */
    void SetJER(std::string const &iovLabel, std::string const &jerSFFile,
      std::string const &jerMCFile);
    
    /**
     * \brief Specifies text files JER smearing for the match-all implicit IOV
     * 
     * This is a special version of the above method to be used when no explicit IOVs are defined.
     */
    void SetJER(std::string const &jerSFFile, std::string const &jerMCFile);
    
private:
    /**
     * \brief Finds IOV for the given label
     * 
     * An empty label has a special meaning and refers to the match-all IOV. The main purpose of
     * this method is error handling.
     */
    IOVParams &GetIOVByLabel(std::string const &label);
    
    /// (Re)creates an object to evaluate JEC for the current IOV
    void UpdateJECEvaluator();
    
    /// (Re)creates an object to evaluate JEC uncertainty for the current IOV
    void UpdateJECUncEvaluator();
    
    /// (Re)creates objects to evaluate effect of JER smearing for the current IOV
    void UpdateJEREvaluator();
    
private:
    /// Parameter sets for all IOVs
    std::vector<IOVParams> iovParams;
    
    /**
     * \brief Map to identify IOVs by labels
     * 
     * Values stored in the map are indices in vector iovParams.
     */
    std::map<std::string, unsigned> iovLabelMap;
    
    /// Flag indicating that a single match-all IOV is used
    bool matchAllMode;
    
    /// Index of the current IOV
    mutable unsigned curIOV;
    
    /**
     * \brief Current run number
     * 
     * Used as a cache to speed up SelectIOV.
     */
    mutable unsigned long curRun;
    
    /**
     * \brief An object that evaluates jet energy corrections with the current IOV
     * 
     * Can be uninitialized if no JEC have been provided.
     */
    std::unique_ptr<FactorizedJetCorrector> jetEnergyCorrector;
    
    /**
     * \brief Objects that compute JEC uncertainties with the current IOV
     * 
     * The vector can be empty if no uncertainties have been specified.
     */
    std::vector<std::unique_ptr<JetCorrectionUncertainty>> jecUncProviders;
    
    /**
     * \brief An object that provides pt resolution in simulation with the current IOV
     * 
     * Can be uninitialized if resolutions have not been specified.
     */
    std::unique_ptr<JME::JetResolution> jerProvider;
    
    /**
     * \brief An object that provides data/MC scale factors for JER with the current IOV
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


template<typename C>
void JetCorrectorService::SetJEC(std::string const &iovLabel, C const &jecFiles_)
{
    static_assert(
      std::is_convertible<typename C::value_type,
      decltype(IOVParams::jecFiles)::value_type>::value,
      "Wrong type of elements of the container");
    
    auto &jecFiles = GetIOVByLabel(iovLabel).jecFiles;
    jecFiles.clear();
    
    for (auto const &jecFile: jecFiles_)
        jecFiles.emplace_back(FileInPath::Resolve("JERC", jecFile));
}


template<typename C>
void JetCorrectorService::SetJEC(C const &jecFiles_)
{
    SetJEC<C>("", jecFiles_);
}


template<typename C>
void JetCorrectorService::SetJECUncertainty(std::string const &iovLabel,
  std::string const &jecUncFile, C const &uncSources)
{
    static_assert(
      std::is_convertible<typename C::value_type,
      decltype(IOVParams::jecUncSources)::value_type>::value,
      "Wrong type of elements of the container");
    
    auto &iov = GetIOVByLabel(iovLabel);
    
    if (jecUncFile != "")
    {
        iov.jecUncFile = FileInPath::Resolve("JERC", jecUncFile);
        iov.jecUncSources.clear();
        
        for (auto const &uncSource: uncSources)
            iov.jecUncSources.emplace_back(uncSource);
    }
    else
    {
        iov.jecUncFile = "";
        iov.jecUncSources.clear();
    }
}


template<typename C>
void JetCorrectorService::SetJECUncertainty(std::string const &jecUncFile, C const &uncSources)
{
    SetJECUncertainty<C>("", jecUncFile, uncSources);
}


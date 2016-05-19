#pragma once

#include <mensura/core/SystService.hpp>

#include <mensura/extensions/JetResolutionFactor.hpp>

#include <mensura/external/JERC/FactorizedJetCorrector.hpp>
#include <mensura/external/JERC/JetCorrectionUncertainty.hpp>

#include <initializer_list>
#include <memory>
#include <string>
#include <vector>


/**
 * \class JetCorrector
 * \brief Applies JEC and performs JER smearing
 * 
 * \warining This class is undergoing a heavy modification. Most of its documentation has not been
 * updated.
 * 
 * The only mandator action is an application of JEC. If required data files are provided, performs
 * JER smearing and, if requested explicitly, evaluates JEC or JER systematical variations. Note
 * that JER smearing is applied for the nominal state as well while the JER systematics controls the
 * amount of smearing.
 */
class JetCorrector
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
    /// Contructor without parameters
    JetCorrector() noexcept;
    
    /**
     * \brief A constructor from files with corrections
     * 
     * Equivalent to a call of the default constructor and execution of AddJECLevel,
     * SetJECUncertainty, and SetJERFile methods.
     */
    // JetCorrector(std::initializer_list<std::string> const &dataFilesJEC,
    //   std::string const &dataFileJECUncertainty = "", std::string const &dataFileJER = "");
    
public:
    /// Returns a newly-initialised copy of this
    // JetCorrector *Clone() const;
    
    /**
     * \brief Adds a data text file with a single level of JEC
     * 
     * JECs are evaluated precisely in the same order as were given to constructor and this method.
     * The file paths are expanded through the FileInPath service.
     */
    void SetJEC(std::initializer_list<std::string> const &jecFiles);
    
    /**
     * \brief (Re)sets data text file with JEC uncertainties
     * 
     * The file path is expanded through the FileInPath service.
     */
    void SetJECUncertainty(std::string const &jecUncFile,
      std::initializer_list<std::string> uncSources = {});
    
    /**
     * \brief (Re)sets data file for JER smearing
     * 
     * The file path is expanded through the FileInPath service.
     */
    void SetJERFile(std::string const &dataFile);
    
    double EvalJECUnc(double const corrPt, double const eta) const;
    
    /**
     * \brief Corrects jet four-momentum
     * 
     * The second argument is the value of the mean angular pt density, rho, which is used in
     * parameterization of L1 JEC.
     * 
     * Throws an exception if the user requests JEC or JER systematical variation while required
     * data files have not been provided. If the jet is already corrected, only raw momentum is
     * used, and the corrected momentum is reset by this method.
     */
    double Eval(Jet const &jet, double rho, SystType syst = SystType::None,
      SystService::VarDirection direction = SystService::VarDirection::Undefined) const;
    
    /// A short-cut for method Eval
    double operator()(Jet const &jet, double rho, SystType syst = SystType::None,
      SystService::VarDirection direction = SystService::VarDirection::Undefined) const;
    
private:
    /**
     * \brief Text files with jet energy corrections
     * 
     * The JEC will be applied precisely in the same order as specified in this collection.
     */
    std::vector<std::string> dataFilesJEC;
    
    /**
     * \brief Text file with JEC uncertainties
     * 
     * The string is empty if the user has not provided the file.
     */
    std::string dataFileJECUncertainty;
    
    /**
     * \brief Data file for JER smearing
     * 
     * The string is empty if the user has not provided the file.
     */
    std::string dataFileJER;
    
    /// An object to evaluate jet energy corrections
    std::unique_ptr<FactorizedJetCorrector> jetEnergyCorrector;
    
    /**
     * \brief An object to evaluate JEC uncertainty
     * 
     * The smart pointer is empty if user has not provided a text file with uncertainties.
     */
    std::vector<std::unique_ptr<JetCorrectionUncertainty>> jecUncAccessors;
    
    /**
     * \brief An object to perform JER smearing
     * 
     * The smart pointer is empty if user has not provided a data file for JER smearing.
     */
    std::unique_ptr<JetResolutionFactor> jerAccessor;
};

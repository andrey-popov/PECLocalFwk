/**
 * \file JetCorrector.hpp
 * \author Andrey Popov
 * 
 * Defines a class to apply JEC and perform JER smearing.
 */

#pragma once

#include <JetCorrectorInterface.hpp>

#include <JetResolutionFactor.hpp>

#include <external/JEC/include/FactorizedJetCorrector.hpp>
#include <external/JEC/include/JetCorrectionUncertainty.hpp>

#include <string>
#include <vector>
#include <memory>


/**
 * \class JetCorrector
 * \brief Applies JEC and performs JER smearing
 * 
 * The only mandator action is an application of JEC. If required data files are provided, performs
 * JER smearing and, if requested explicitly, evaluates JEC or JER systematical variations. Note
 * that JER smearing is applied for the nominal state as well while the JER systematics controls the
 * amount of smearing.
 */
class JetCorrector: public JetCorrectorInterface
{
public:
    /// Contructor without parameters
    JetCorrector() noexcept;
    
    /**
     * \brief A constructor from files with corrections
     * 
     * Equivalent to a call of the default constructor and execution of AddJECLevel,
     * SetJECUncertainty, and SetJERFile methods.
     */
    JetCorrector(std::vector<std::string> const &dataFilesJEC,
     std::string const &dataFileJECUncertainty = "", std::string const &dataFileJER = "") noexcept;
    
public:
    /// Returns a newly-initialised copy of this
    virtual JetCorrectorInterface *Clone() const noexcept;
    
    /**
     * \brief Adds a data text file with a single level of JEC
     * 
     * JECs are evaluated precisely in the same order as were given to constructor and this method.
     * The file paths are expanded through the FileInPath service.
     */
    void AddJECLevel(std::string const &dataFile) noexcept;
    
    /**
     * \brief (Re)sets data text file with JEC uncertainties
     * 
     * The file path is expanded through the FileInPath service.
     */
    void SetJECUncertainty(std::string const &dataFile) noexcept;
    
    /**
     * \brief (Re)sets data file for JER smearing
     * 
     * The file path is expanded through the FileInPath service.
     */
    void SetJERFile(std::string const &dataFile) noexcept;
    
    /**
     * \brief Initialises the object
     * 
     * Reads files with parameters for corrections and constructs relevant objects.
     * 
     * For further details refer to the documentation of the overriden method.
     */
    virtual void Init();
    
    /**
     * \brief Corrects jet four-momentum
     * 
     * Throws an exception if the user requests JEC or JER systematical variation while required
     * data files have not been provided. If the jet is already corrected, only raw momentum is
     * used, and the corrected momentum is reset by this method.
     * 
     * For further details refer to the documentation of the overriden method.
     */
    virtual void Correct(Jet &jet, double rho, SystVariation syst = SystVariation());
    
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
     * The smart pointer is empty if the user has not provided a text file with uncertainties.
     */
    std::unique_ptr<JetCorrectionUncertainty> jecUncertaintyAccessor;
    
    /**
     * \brief An object to perform JER smearing
     * 
     * The smart pointer is empry if the user has not provided a data file for JER smearing.
     */
    std::unique_ptr<JetResolutionFactor> jerAccessor;
};

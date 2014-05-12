/**
 * \file JetCorrector.hpp
 * \author Andrey Popov
 * 
 * 
 */

#pragma once

#include <JetCorrectorInterface.hpp>

#include <external/JEC/include/FactorizedJetCorrector.hpp>
#include <external/JEC/include/JetCorrectionUncertainty.hpp>

#include <string>
#include <vector>
#include <memory>


/**
 * \class JetCorrector
 * 
 * 
 */
class JetCorrector: public JetCorrectorInterface
{
public:
    /// Contructor without parameters
    JetCorrector() noexcept;
    
    /**
     * \brief A constructor from files with corrections
     * 
     * Equivalent to a call of the default constructor and execution of AddJECLevel and
     * SetJECUncertainty methods.
     */
    JetCorrector(std::vector<std::string> const &srcFilesJEC,
     std::string const &srcFileJECUncertainty) noexcept;
    
public:
    virtual JetCorrectorInterface *Clone() const noexcept;
    
    /**
     * \brief Adds a source text file with a single level of JEC
     * 
     * JECs are evaluated precisely in the same order as were given to constructor and this method.
     * The file paths are expanded through the FileInPath service.
     */
    void AddJECLevel(std::string const &srcFile) noexcept;
    
    /**
     * \brief (Re)sets source text file with JEC uncertainties
     * 
     * The file path is expanded through the FileInPath service.
     */
    void SetJECUncertainty(std::string const &srcFile) noexcept;
    
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
     * Throws an exception if the user requests JEC systematics while JEC uncertainties have not
     * been set up.
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
    std::vector<std::string> srcFilesJEC;
    
    /// Text file with JEC uncertainties
    std::string srcFileJECUncertainty;
    
    /// An object to evaluate jet energy corrections
    std::unique_ptr<FactorizedJetCorrector> jetEnergyCorrector;
    
    /**
     * \brief An object to evaluate JEC uncertainty
     * 
     * The smart pointer is empty if the user has not provided a text file with uncertainties.
     */
    std::unique_ptr<JetCorrectionUncertainty> jecUncertaintyAccessor;
};

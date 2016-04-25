#pragma once

#include <mensura/core/Service.hpp>

#include <initializer_list>
#include <map>
#include <string>
#include <utility>


/**
 * \class SystService
 * \brief A service to report requested systematic variation to plugins
 * 
 * Types of systematic uncertainties are described by arbitrary text labels. A new label must first
 * be registered, then a corresponding systematic variation can be requested using method Set.
 * Several types of uncertainty are registered automatically: "None", "JEC", "JER", "METUncl",
 * "WeightPileUp", "WeightBTag".
 */
class SystService: public Service
{
public:
    /// Supported directions for systematic variations
    enum class VarDirection
    {
        Undefined,
        Up,
        Down
    };
    
public:
    /**
     * \brief Creates a new service with the given name
     * 
     * Registers the default set of systematic uncertainties (see documentation to the class). Sets
     * the requested variation to "None".
     */
    SystService(std::string name = "Systematics");
    
    /**
     * \brief Creates a new service with a default name "Systematics"
     * 
     * Registers the default set of systematic uncertainties (see documentation to the class). Sets
     * the requested variation to the given one.
     */
    SystService(std::string const &label, VarDirection direction);
    
public:
    /// Creates a newly configured clone
    virtual Service *Clone() const override;
    
    /**
     * \brief Registers a new type of systematic uncertainty
     * 
     * The uncertainty might have a direction (e.g. JEC) or not (uncertainties implemented as
     * event weights). This method does not set an uncertainty, only adds a new allowed type.
     */
    void Register(std::string const &label, bool hasDirection);
    
    /**
     * \brief Requests the given variation
     * 
     * The uncertainty with the given label must have been registered beforehand. If it is not the
     * case, an exception is thrown. If the provided variation does not agree with what was set
     * with flag hasDirection in method Register, an exception is thrown.
     * 
     * If this method is called several times, only the last call has an effect.
     */
    void Set(std::string const &label, VarDirection direction = VarDirection::Undefined);
    
    /**
     * \brief A version of the above method to request multiple variations
     * 
     * Mostly useful for uncertainties implemented as event weights since usually they should be
     * calculated together.
     */
    void Set(std::initializer_list<std::pair<std::string, VarDirection>> variations);
    
    /**
     * \brief Tests if a systematic variation with the given label has been requested
     * 
     * If the variation is not known (has not been registered beforehand), an exception is thrown.
     * The first value of the returned pair indicates whether the variation has been requested, the
     * second value is the corresponding direction.
     */
    std::pair<bool, VarDirection> Test(std::string const &label) const;
    
private:
    /**
     * \brief Registered systematic uncertainties
     * 
     * The key of the map is the label of an uncertainty, and the value shows whether this
     * uncertainty is directional.
     */
    std::map<std::string, bool> allowedVariations;
    
    /**
     * \brief Requested systematic variations
     * 
     * The key of the map is the label of an uncertainty and the value is the requested direction
     * of the corresponding variation.
     */
    std::map<std::string, VarDirection> requestedVariations;
};

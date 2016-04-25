#include <mensura/core/SystService.hpp>

#include <limits>
#include <sstream>
#include <stdexcept>


SystService::SystService(std::string name /*= "Systematics"*/):
    Service(name)
{
    // Register systematic uncertainties used in the framework
    allowedVariations["None"] = false;
    
    allowedVariations["JEC"] = true;
    allowedVariations["JER"] = true;
    allowedVariations["METUncl"] = true;
    
    allowedVariations["WeightPileUp"] = false;
    allowedVariations["WeightBTag"] = false;
    
    
    // By default, set variation "None"
    requestedVariations["None"] = VarDirection::Undefined;
}


SystService::SystService(std::string const &label, VarDirection direction):
    SystService("Systematics")
{
    Set(label, direction);
}


Service *SystService::Clone() const
{
    return new SystService(*this);
}


void SystService::Register(std::string const &label, bool hasDirection)
{
    // Make sure that a systematics with the given name has not been registered before
    auto const res = allowedVariations.find(label);
    
    if (res != allowedVariations.end())
    {
        std::ostringstream message;
        message << "SystService[\"" << GetName() << "\"]::Register: Systematic uncertainty of "
          "type \"" << label << "\" has already been registered.";
        throw std::runtime_error(message.str());
    }
    
    
    // Save the new type
    allowedVariations[label] = hasDirection;
}


void SystService::Set(std::string const &label,
  VarDirection direction /*= VarDirection::Undefined*/)
{
    Set({make_pair(label, direction)});
}


void SystService::Set(std::initializer_list<std::pair<std::string, VarDirection>> variations)
{
    // Clean any variations requested before
    requestedVariations.clear();
    
    
    for (auto const &var: variations)
    {
        // Make sure that the current variation is supported and its direction is defined correctly
        auto const allowedDBIt = allowedVariations.find(var.first);
        
        if (allowedDBIt == allowedVariations.end())
        {
            std::ostringstream message;
            message << "SystService[\"" << GetName() << "\"]::Set: Systematic uncertainty of " <<
              "type \"" << allowedDBIt->first << "\" is not known.";
            throw std::runtime_error(message.str());
        }
        
        if (allowedDBIt->second == false and var.second != VarDirection::Undefined)
        {
            std::ostringstream message;
            message << "SystService[\"" << GetName() << "\"]::Set: Non-trivial direction is " <<
              "specified for systematic variation \"" << allowedDBIt->first <<
              "\", which does not support direction.";
            throw std::runtime_error(message.str());
        }
        
        if (allowedDBIt->second == true and var.second == VarDirection::Undefined)
        {
            std::ostringstream message;
            message << "SystService[\"" << GetName() << "\"]::Set: No direction is specified " <<
              "for systematic variation \"" << allowedDBIt->first << "\" while this is required.";
            throw std::runtime_error(message.str());
        }
        
        
        // Make sure this systematics has not been requested already
        auto const requestedIt = requestedVariations.find(var.first);
        
        if (requestedIt != requestedVariations.end())
        {
            std::ostringstream message;
            message << "SystService[\"" << GetName() << "\"]::Set: Variation of type \"" <<
              var.first << "\" has already been requested.";
            throw std::runtime_error(message.str());
        }
        
        
        // Finally, add to the set of requested variations
        requestedVariations[var.first] = var.second;
    }
}


std::pair<bool, SystService::VarDirection> SystService::Test(std::string const &label) const
{
    // Make sure type of the given variation is known
    auto const allowedDBIt = allowedVariations.find(label);
    
    if (allowedDBIt == allowedVariations.end())
    {
        std::ostringstream message;
        message << "SystService[\"" << GetName() << "\"]::Test: Systematic variation of type \"" <<
          label << "\" is not known.";
        throw std::runtime_error(message.str());
    }
    
    
    // Check if the given variation has been requested
    auto const requestedIt = requestedVariations.find(label);
    
    if (requestedIt == requestedVariations.end())
        return {false, VarDirection::Undefined};
    else
        return {true, requestedIt->second};
}

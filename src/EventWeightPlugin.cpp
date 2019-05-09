#include <mensura/EventWeightPlugin.hpp>

#include <stdexcept>
#include <sstream>


EventWeightPlugin::EventWeightPlugin(std::string const &name_):
    AnalysisPlugin(name_)
{}


double EventWeightPlugin::GetWeight() const
{
    return weights.at(0);
}


unsigned EventWeightPlugin::GetNumVariations() const noexcept
{
    return (weights.size() - 1) / 2;
}


double EventWeightPlugin::GetWeightUp(unsigned iSource) const
{
    // Make sure the index makes sense
    if (iSource >= GetNumVariations())
    {
        std::ostringstream message;
        message << "EventWeightPlugin[\"" << GetName() << "\"]::GetWeightUp: Trying to access "
          "systematic variation for source #" << iSource << " while only " << GetNumVariations() <<
          " sources are defined.";
        
        throw std::out_of_range(message.str());
    }
    
    
    // Return the variation
    return weights.at(1 + iSource * 2);
}


double EventWeightPlugin::GetWeightDown(unsigned iSource) const
{
    // Make sure the index makes sense
    if (iSource >= GetNumVariations())
    {
        std::ostringstream message;
        message << "EventWeightPlugin[\"" << GetName() << "\"]::GetWeightDown: Trying to access "
          "systematic variation for source #" << iSource << " while only " << GetNumVariations() <<
          " sources are defined.";
        
        throw std::out_of_range(message.str());
    }
    
    
    // Return the variation
    return weights.at(2 + iSource * 2);
}


std::vector<double> const &EventWeightPlugin::GetWeights() const noexcept
{
    return weights;
}

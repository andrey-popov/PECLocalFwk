#include <EventWeightPlugin.hpp>

#include <stdexcept>
#include <sstream>


using namespace std;


EventWeightPlugin::EventWeightPlugin(string const &name_):
    Plugin(name_)
{}


EventWeightPlugin::EventWeightPlugin(EventWeightPlugin const &src) noexcept:
    Plugin(src),
    weights(src.weights)
{}


EventWeightPlugin::~EventWeightPlugin() noexcept
{}


double EventWeightPlugin::GetWeight() const
{
    return weights.at(0);
}


unsigned EventWeightPlugin::GetNumSystSources() const noexcept
{
    return (weights.size() - 1) / 2;
}


double EventWeightPlugin::GetWeightUp(unsigned iSource) const
{
    // Make sure the index makes sense
    if (iSource >= GetNumSystSources())
    {
        ostringstream ost;
        ost << "EventWeightPlugin::GetWeightUp: Trying to access systematical variation for "
         "source #" << iSource << " while only " << GetNumSystSources() <<
         " sources are available.";
        
        throw logic_error(ost.str());
    }
    
    
    // Return the variation
    return weights.at(1 + iSource * 2);
}


double EventWeightPlugin::GetWeightDown(unsigned iSource) const
{
    // Make sure the index makes sense
    if (iSource >= GetNumSystSources())
    {
        ostringstream ost;
        ost << "EventWeightPlugin::GetWeightDown: Trying to access systematical variation for "
         "source #" << iSource << " while only " << GetNumSystSources() <<
         " sources are available.";
        
        throw logic_error(ost.str());
    }
    
    
    // Return the variation
    return weights.at(2 + iSource * 2);
}


vector<double> const &EventWeightPlugin::GetWeights() const noexcept
{
    return weights;
}

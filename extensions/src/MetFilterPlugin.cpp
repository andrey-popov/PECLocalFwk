#include <MetFilterPlugin.hpp>

#include <Processor.hpp>

#include <string>
#include <sstream>


using namespace std;


/*
 * A function to add the MET threshold to the plugin's name. It is needed since the user might need
 * to create several instances of this plugin with difference thresholds, but (s)he will not be
 * to do so if all the plugins have identical names.
 */
string BuildPluginName(string const &baseName, double met)
{
    ostringstream ost;
    ost << baseName << met;
    
    return ost.str();
}


MetFilterPlugin::MetFilterPlugin(double threshold_):
    Plugin(BuildPluginName("MetFilter", threshold_)),
    threshold(threshold_)
{}


Plugin *MetFilterPlugin::Clone() const
{
    return new MetFilterPlugin(threshold);
}


void MetFilterPlugin::BeginRun(Dataset const &)
{
    // Save pointer to the reader plugin
    reader = dynamic_cast<PECReaderPlugin const *>(processor->GetPluginBefore("Reader", name));
}


bool MetFilterPlugin::ProcessEvent()
{
    Candidate const &met = (*reader)->GetMET();
    
    return (met.Pt() > threshold);
}

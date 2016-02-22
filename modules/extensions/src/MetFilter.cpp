#include <PECFwk/extensions/MetFilter.hpp>

#include <PECFwk/core/JetMETReader.hpp>
#include <PECFwk/core/Processor.hpp>

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


MetFilter::MetFilter(double threshold_):
    AnalysisPlugin(BuildPluginName("MetFilter", threshold_)),
    metPluginName("JetMET"),
    threshold(threshold_)
{}


Plugin *MetFilter::Clone() const
{
    return new MetFilter(threshold);
}


void MetFilter::BeginRun(Dataset const &)
{
    // Save pointer to plugin that produces jets
    metPlugin = dynamic_cast<JetMETReader const *>(
      GetMaster().GetPluginBefore(metPluginName, GetName()));
}


bool MetFilter::ProcessEvent()
{
    MET const &met = metPlugin->GetMET();
    
    return (met.Pt() > threshold);
}

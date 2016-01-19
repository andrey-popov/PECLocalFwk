#include <JetPtFilterPlugin.hpp>

#include <Processor.hpp>

#include <string>
#include <sstream>
#include <stdexcept>


using namespace std;


/*
 * A function to add number of jets and threshold to the plugin's name. It is needed since the user
 * might need to create several instances of this plugin with difference thresholds, but (s)he will
 * not allowed to do it if all the plugins have identical names.
 */
string BuildPluginName(string const &baseName, unsigned nJets, double pt)
{
    ostringstream ost;
    ost << baseName << nJets << pt;
    
    return ost.str();
}


JetPtFilterPlugin::JetPtFilterPlugin(unsigned minNumJets_, double ptThreshold_):
    AnalysisPlugin(BuildPluginName("JetPtFilter", minNumJets_, ptThreshold_)),
    minNumJets(minNumJets_), ptThreshold(ptThreshold_)
{
    if (minNumJets == 0)
        throw logic_error("JetPtFilterPlugin::JetPtFilterPlugin: Number of jets must be sctrictly "
         "positive.");
}


Plugin *JetPtFilterPlugin::Clone() const
{
    return new JetPtFilterPlugin(minNumJets, ptThreshold);
}


void JetPtFilterPlugin::BeginRun(Dataset const &)
{
    // Save pointer to the reader plugin
    reader = dynamic_cast<PECReaderPlugin const *>(master->GetPluginBefore("Reader", name));
}


bool JetPtFilterPlugin::ProcessEvent()
{
    auto const &jets = (*reader)->GetJets();
    auto const &softJets = (*reader)->GetAdditionalJets();
    
    
    if (minNumJets - 1 < jets.size())
        return (jets.at(minNumJets - 1).Pt() > ptThreshold);
    
    if (minNumJets - 1 < jets.size() + softJets.size())
        return (softJets.at(minNumJets - 1 - jets.size()).Pt() > ptThreshold);
    
    
    // If control reaches this point, there are less than minNumJets jets in both collections
    return false;
}

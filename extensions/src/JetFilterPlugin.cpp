#include <JetFilterPlugin.hpp>

#include <Processor.hpp>

#include <sstream>
#include <stdexcept>


using namespace std;


/*
 * A function to add number of jets and threshold to the plugin's name. It is needed since the user
 * might need to create several instances of this plugin with difference thresholds, but (s)he will
 * not allowed to do it if all the plugins have identical names.
 */
string BuildPluginName(string const &baseName, unsigned minNumJets, unsigned maxNumJets) noexcept
{
    ostringstream ost;
    ost << baseName << "_" << minNumJets << "_" << maxNumJets;
    
    return ost.str();
}


JetFilterPlugin::JetFilterPlugin(string const &name_,
 function<bool(Jet const &)> const &selection_,
 unsigned minNumJets_, unsigned maxNumJets_ /*= -1*/) noexcept:
    Plugin(name_),
    selection(selection_),
    minNumJets(minNumJets_), maxNumJets(maxNumJets_)
{}


JetFilterPlugin::JetFilterPlugin(function<bool(Jet const &)> const &selection_,
 unsigned minNumJets_, unsigned maxNumJets_ /*= -1*/) noexcept:
    Plugin(BuildPluginName("JetFilter", minNumJets, maxNumJets)),
    selection(selection_),
    minNumJets(minNumJets_), maxNumJets(maxNumJets_)
{}


JetFilterPlugin::JetFilterPlugin(JetFilterPlugin const &src) noexcept:
    Plugin(src),
    selection(src.selection),
    minNumJets(src.minNumJets), maxNumJets(src.maxNumJets)
{}


Plugin *JetFilterPlugin::Clone() const noexcept
{
    return new JetFilterPlugin(*this);
}


void JetFilterPlugin::BeginRun(Dataset const &)
{
    // Save pointer to the reader plugin
    reader = dynamic_cast<PECReaderPlugin const *>(processor->GetPluginBefore("Reader", name));
}


bool JetFilterPlugin::ProcessEvent()
{
    auto const &jets = (*reader)->GetJets();
    auto const &softJets = (*reader)->GetAdditionalJets();
    
    
    // Count the number of jets that pass the selection
    unsigned nPassed = 0;
    
    for (auto const &jetCollection: {jets, softJets})
        for (Jet const &j: jetCollection)
            if (selection(j))
                ++nPassed;
    
    
    return (nPassed >= minNumJets and nPassed <= maxNumJets);
}

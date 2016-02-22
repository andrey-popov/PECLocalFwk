#include <PECFwk/extensions/JetFunctorFilter.hpp>

#include <PECFwk/core/JetMETReader.hpp>
#include <PECFwk/core/Processor.hpp>

#include <sstream>
#include <stdexcept>


using namespace std;


JetFunctorFilter::JetFunctorFilter(string const &name_,
  function<bool(Jet const &)> const &selector_,
  unsigned minNumJets_, unsigned maxNumJets_ /*= -1*/) noexcept:
    AnalysisPlugin(name_),
    jetPluginName("JetMET"), jetPlugin(nullptr),
    selector(selector_),
    minNumJets(minNumJets_), maxNumJets(maxNumJets_)
{}


JetFunctorFilter::JetFunctorFilter(function<bool(Jet const &)> const &selector_,
  unsigned minNumJets_, unsigned maxNumJets_ /*= -1*/) noexcept:
    AnalysisPlugin("JetFunctorFilter"),
    jetPluginName("JetMET"), jetPlugin(nullptr),
    selector(selector_),
    minNumJets(minNumJets_), maxNumJets(maxNumJets_)
{}


JetFunctorFilter::JetFunctorFilter(JetFunctorFilter const &src) noexcept:
    AnalysisPlugin(src),
    selector(src.selector),
    minNumJets(src.minNumJets), maxNumJets(src.maxNumJets)
{}


void JetFunctorFilter::BeginRun(Dataset const &)
{
    // Save pointer to plugin that produces jets
    jetPlugin = dynamic_cast<JetMETReader const *>(
      GetMaster().GetPluginBefore(jetPluginName, GetName()));
}


Plugin *JetFunctorFilter::Clone() const noexcept
{
    return new JetFunctorFilter(*this);
}


bool JetFunctorFilter::ProcessEvent()
{
    auto const &jets = jetPlugin->GetJets();
    
    
    // Count the number of jets that pass the selection
    unsigned nPassed = 0;
    
    for (Jet const &j: jets)
        if (selector(j))
            ++nPassed;
    
    
    return (nPassed >= minNumJets and nPassed <= maxNumJets);
}

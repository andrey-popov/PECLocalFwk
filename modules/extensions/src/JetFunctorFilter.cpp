#include <mensura/extensions/JetFunctorFilter.hpp>

#include <mensura/core/JetMETReader.hpp>
#include <mensura/core/Processor.hpp>

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


void JetFunctorFilter::BeginRun(Dataset const &)
{
    // Save pointer to plugin that produces jets
    jetPlugin = dynamic_cast<JetMETReader const *>(GetDependencyPlugin(jetPluginName));
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

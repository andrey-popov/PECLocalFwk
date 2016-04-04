#include <PECFwk/extensions/JetFilter.hpp>

#include <PECFwk/core/BTagWPService.hpp>
#include <PECFwk/core/JetMETReader.hpp>
#include <PECFwk/core/PhysicsObjects.hpp>
#include <PECFwk/core/Processor.hpp>


JetFilter::SelectionBin::SelectionBin(unsigned minJets_, unsigned maxJets_, unsigned minTags_,
  unsigned maxTags_) noexcept:
    minJets(minJets_), maxJets(maxJets_),
    minTags(minTags_), maxTags(maxTags_)
{}


bool JetFilter::SelectionBin::Contains(unsigned nJets, unsigned nTags) const
{
    return (nJets >= minJets and nTags >= minTags and nJets <= maxJets and nTags <= maxTags);
}



JetFilter::JetFilter(std::string const name, double minPt_, BTagger const &bTagger_) noexcept:
    AnalysisPlugin(name),
    jetPluginName("JetMET"), jetPlugin(nullptr),
    bTagWPServiceName("BTagWP"), bTagWPService(nullptr),
    minPt(minPt_), bTagger(bTagger_)
{}


JetFilter::JetFilter(double minPt_, BTagger const &bTagger_) noexcept:
    AnalysisPlugin("JetFilter"),
    jetPluginName("JetMET"), jetPlugin(nullptr),
    bTagWPServiceName("BTagWP"), bTagWPService(nullptr),
    minPt(minPt_), bTagger(bTagger_)
{}


JetFilter::~JetFilter() noexcept
{}


void JetFilter::AddSelectionBin(unsigned minJets, unsigned maxJets, unsigned minTags,
  unsigned maxTags)
{
    bins.emplace_back(minJets, maxJets, minTags, maxTags);
}


void JetFilter::AddSelectionBin(unsigned nJets, unsigned nTags)
{
    bins.emplace_back(nJets, nJets, nTags, nTags);
}


void JetFilter::BeginRun(Dataset const &)
{
    // Save pointer to plugin that produces jets
    jetPlugin = dynamic_cast<JetMETReader const *>(GetDependencyPlugin(jetPluginName));
    
    // Save pointer to service that provides b-tagging thresholds
    bTagWPService = dynamic_cast<BTagWPService const *>(GetMaster().GetService(bTagWPServiceName));
}


Plugin *JetFilter::Clone() const
{
    return new JetFilter(*this);
}


bool JetFilter::ProcessEvent()
{
    // Count jets and b tags
    unsigned nJets = 0, nTags = 0;
    
    for (auto const &j: jetPlugin->GetJets())
    {
        if (j.Pt() <= minPt)
        {
            // Jets are ordered in pt, so can stop the loop
            break;
        }
        
        ++nJets;
        
        if (bTagWPService->IsTagged(bTagger, j))
            ++nTags;
    }
    
    
    // Check if the found multiplicities satisfy any selection bin
    for (auto const &b: bins)
    {
        if (b.Contains(nJets, nTags))
            return true;
    }
    
    
    // If control had reached this point, the event does not satisfy any selection bin
    return false;
}

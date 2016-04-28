#include <mensura/extensions/BTagWeight.hpp>

#include <mensura/core/BTagWPService.hpp>
#include <mensura/core/JetMETReader.hpp>
#include <mensura/core/PhysicsObjects.hpp>
#include <mensura/core/Processor.hpp>
#include <mensura/extensions/BTagEffService.hpp>


BTagWeight::BTagWeight(std::string const &name, BTagger bTagger_,
  double minPt_ /*= 0.*/):
    EventWeightPlugin(name),
    jetPluginName("JetMET"), jetPlugin(nullptr),
    bTagWPServiceName("BTagWP"), bTagWPService(nullptr),
    bTagEffServiceName("BTagEff"), bTagEffService(nullptr),
    bTagSFServiceName("BTagSF"), bTagSFService(nullptr),
    bTagger(bTagger_),
    minPt(minPt_),
    evalSystematics(false)
{}


BTagWeight::BTagWeight(BTagger bTagger_, double minPt_ /*= 0.*/):
    EventWeightPlugin("BTagWeight"),
    jetPluginName("JetMET"), jetPlugin(nullptr),
    bTagWPServiceName("BTagWP"), bTagWPService(nullptr),
    bTagEffServiceName("BTagEff"), bTagEffService(nullptr),
    bTagSFServiceName("BTagSF"), bTagSFService(nullptr),
    bTagger(bTagger_),
    minPt(minPt_),
    evalSystematics(false)
{}


BTagWeight::~BTagWeight() noexcept
{}


void BTagWeight::BeginRun(Dataset const &)
{
    // Save pointer to plugin that produces jets
    jetPlugin = dynamic_cast<JetMETReader const *>(GetDependencyPlugin(jetPluginName));
    
    // Save pointers to services that provide b-tagging thresholds, efficiencies, and scale factors
    bTagWPService = dynamic_cast<BTagWPService const *>(GetMaster().GetService(bTagWPServiceName));
    bTagEffService =
      dynamic_cast<BTagEffService const *>(GetMaster().GetService(bTagEffServiceName));
    bTagSFService = dynamic_cast<BTagSFService const *>(GetMaster().GetService(bTagSFServiceName));
    
    
    // Initialize weights
    if (evalSystematics)
        weights.assign(5, 0.);
    else
        weights.assign(1, 0.);
}


Plugin *BTagWeight::Clone() const
{
    return new BTagWeight(*this);
}


void BTagWeight::RequestSystematics(bool on /*= true*/)
{
    evalSystematics = on;
}


double BTagWeight::CalcWeight(Variation var /*=Variation::Nominal*/) const
{
    // The weight will be constructed following this recipe [1]. It will be calculated as a product
    //of per-jet factors. These factors are of the order of 1, and for this reason it is fine to
    //simply multiply them instead of calculating a sum of logarithms, which is more stable in case
    //of small multipliers
    //[1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagSFMethods?rev=27#1a_Event_reweighting_using_scale
    double weight = 1.;
    
    
    // Loop over jets in the current event
    for (auto const &jet: jetPlugin->GetJets())
    {
        // Skip jets that fail the pt cut or fall outside of the tracker acceptance
        if (jet.Pt() < minPt or std::fabs(jet.Eta()) > BTagger::GetMaxPseudorapidity())
            continue;
        
        
        // Precalculate b-tagging scale factor for the current jet
        double const sf =
          bTagSFService->GetScaleFactor(jet, TranslateVariation(var, jet.GetParentID()));
        
        
        // Update the weight
        if (bTagWPService->IsTagged(bTagger, jet))
            weight *= sf;
        else
        {
            // Only in this case the b-tagging efficiency is needed. Calculate it
            double const eff = bTagEffService->GetEfficiency(bTagger, jet);
            
            if (eff < 1.)
                weight *= (1. - sf * eff) / (1. - eff);
            //^ The above formula does not work if eff == 1. This should be a very rear event, and
            //it is possible if only the efficiencies were measured after an event selection that
            //does not enclose the event selection applied at the moment, or if efficiencies from a
            //wrong dataset are applied. Anyway, an untagged jet with eff == 1. is ignored. This is
            //an ad-hoc solution motivated only in the case of sf == 1.
        }
    }
    
    
    return weight;
}


bool BTagWeight::ProcessEvent()
{
    weights.at(0) = CalcWeight(Variation::Nominal);
    
    if (evalSystematics)
    {
        weights.at(1) = CalcWeight(Variation::TagRateUp);
        weights.at(2) = CalcWeight(Variation::TagRateDown);
        weights.at(3) = CalcWeight(Variation::MistagRateUp);
        weights.at(4) = CalcWeight(Variation::MistagRateDown);
    }
    
    return true;
}


BTagSFService::Variation BTagWeight::TranslateVariation(Variation var, int jetPdgId)
{
    unsigned const absFlavour = abs(jetPdgId);
    
    if (absFlavour == 5 or absFlavour == 4)
    {
        if (var == Variation::TagRateUp)
            return BTagSFService::Variation::Up;
        else if (var == Variation::TagRateDown)
            return BTagSFService::Variation::Down;
    }
    else  // light-flavour and gluon jets
    {
        if (var == Variation::MistagRateUp)
            return BTagSFService::Variation::Up;
        else if (var == Variation::MistagRateDown)
            return BTagSFService::Variation::Down;
    }
    
    // If control has reached this point, it is either pure nominal or mismatched variation (like
    //requesting TagRateUp for a gluon jet)
    return BTagSFService::Variation::Nominal;
}

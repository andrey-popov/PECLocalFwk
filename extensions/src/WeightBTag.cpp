#include <WeightBTag.hpp>


using namespace std;


WeightBTag::WeightBTag(shared_ptr<BTagger const> &bTagger_, BTagger::WorkingPoint workingPoint_,
 unique_ptr<BTagEffInterface> &efficiencies_, unique_ptr<BTagSFInterface> &scaleFactors_):
    WeightBTagInterface(),
    bTagger(bTagger_), efficiencies(move(efficiencies_)), scaleFactors(move(scaleFactors_)),
    workingPoint(workingPoint_)
{}


WeightBTag::WeightBTag(shared_ptr<BTagger const> &bTagger_, BTagger::WorkingPoint workingPoint_,
 BTagEffInterface &efficiencies_, BTagSFInterface &scaleFactors_):
    WeightBTagInterface(),
    bTagger(bTagger_),
    efficiencies(efficiencies_.Clone()), scaleFactors(scaleFactors_.Clone()),
    workingPoint(workingPoint_)
{}


WeightBTag::WeightBTag(shared_ptr<BTagger const> &bTagger_,
 unique_ptr<BTagEffInterface> &efficiencies_, unique_ptr<BTagSFInterface> &scaleFactors_):
    WeightBTag(bTagger_, bTagger_->GetWorkingPoint(), efficiencies_, scaleFactors_)
{}


WeightBTag::WeightBTag(shared_ptr<BTagger const> &bTagger_,
 BTagEffInterface &efficiencies_, BTagSFInterface &scaleFactors_):
    WeightBTag(bTagger_, bTagger_->GetWorkingPoint(), efficiencies_, scaleFactors_)
{}


WeightBTag::WeightBTag(WeightBTag const &src):
    WeightBTagInterface(src),
    bTagger(src.bTagger),  // Points to the same object
    efficiencies(src.efficiencies->Clone()),
    scaleFactors(src.scaleFactors->Clone()),
    workingPoint(src.workingPoint)
{}


WeightBTag::WeightBTag(WeightBTag &&src) noexcept:
    WeightBTagInterface(move(src)),
    bTagger(move(src.bTagger)),
    efficiencies(move(src.efficiencies)),
    scaleFactors(move(src.scaleFactors)),
    workingPoint(src.workingPoint)
{}


WeightBTag::~WeightBTag() noexcept
{}


WeightBTagInterface *WeightBTag::Clone() const
{
    return new WeightBTag(*this);
}


void WeightBTag::LoadPayload(Dataset const &dataset)
{
    efficiencies->LoadPayload(dataset);
}


double WeightBTag::CalcWeight(vector<Jet> const &jets, Variation var /*=Variation::Nominal*/) const
{
    // The weight will be constructed following this recipe [1]. It will be calculated as a product
    //of per-jet factors. These factors are of the order of 1, and for this reason it is fine to
    //simply multiply them instead of calculating a sum of logarithms, which is more stable in case
    //of small multipliers
    //[1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagSFMethods#1a_Event_reweighting_using_scale
    double weight = 1.;
    
    
    // Loop over the jets
    for (auto const &jet: jets)
    {
        // Skip jets outside the tracker acceptance
        if (fabs(jet.Eta()) > BTagSFInterface::GetMaxPseudorapidity())
            continue;
        
        
        // Precalculate b-tagging scale factor for the current jet
        double const sf = scaleFactors->GetScaleFactor(workingPoint, jet,
         TranslateVariation(var, jet.GetParentID()));
        
        
        // Update the weight
        if (bTagger->IsTagged(workingPoint, jet))
            weight *= sf;
        else
        {
            // Only in this case the b-tagging efficiency is needed. Calculate it
            double const eff = efficiencies->GetEfficiency(workingPoint, jet);
            
            if (eff < 1.)
                weight *= (1. - sf * eff) / (1. - eff);
            //^ The above formula does not work if eff == 1. It should be a very rear event and is
            //possible if only the efficiencies were measured after an event selection that does not
            //enclose the event selection applied at the moment, or if efficiencies from a wrong
            //dataset are applied. Nevertheless, an untagged jet with eff == 1. is ignored. This is
            //an ad-hoc solution motivated only in the case of sf == 1.
        }
    }
    
    
    return weight;
}

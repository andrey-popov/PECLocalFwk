#include <WeightBTag.hpp>


using namespace std;


WeightBTag::WeightBTag(BTagger const *bTagger_, BTagEffInterface *efficiencies_,
 BTagSFInterface *scaleFactors_):
    bTagger(bTagger_), efficiencies(efficiencies_), scaleFactors(scaleFactors_)
{}


WeightBTagInterface *WeightBTag::Clone() const
{
    // This is a placeholder!
    return nullptr;
}


void WeightBTag::LoadPayload(Dataset const &dataset)
{
    efficiencies->LoadPayload(dataset);
}


double WeightBTag::CalcWeight(vector<Jet> const &jets, Variation var /*=Variation::Nominal*/) const
{
    // Logarithms of probabilities that the given tag configuration takes place with data-like or
    //MC-like b-tagging efficiencies. Since the probabilities for several jets will be multiplied,
    //it is more robust to operate with logarithms
    double logProbData = 0., logProbMC = 0.;
    
    
    // Loop over the jets
    for (auto const &jet: jets)
    {
        // Skip jets outside the tracker acceptance
        if (fabs(jet.Eta()) > BTagSFInterface::GetMaxPseudorapidity())
            continue;
        
        
        // Precalculate b-tagging efficiency and scale factor with the current jet
        double const eff = efficiencies->GetEfficiency(jet);
        double const sf = scaleFactors->GetScaleFactor(jet,
         TranslateVariation(var, jet.GetParentID()));
        
        if (bTagger->IsTagged(jet))
        {
            logProbMC += log(eff);
            logProbData += log(eff * sf);
        }
        else
        {
            logProbMC += log(1. - eff);
            logProbData += log(1. - eff * sf);
        }
        //^ It is worth noting that despite both the efficiencies and the scale factors can be zero
        //(outside the tracker acceptance) the arguments of logarithms are always positive since
        //a tagged jet has non-zero efficiency and scale factor
    }
    
    
    // This is it. Much simpler than in previous version of the algorithm
    return exp(logProbData - logProbMC);
}

#include <WeightBTag.hpp>


using namespace std;


WeightBTag::WeightBTag(shared_ptr<BTagger const> &bTagger_, BTagger::WorkingPoint workingPoint_,
 unique_ptr<BTagEffInterface> &efficiencies_, unique_ptr<BTagSFInterface> &scaleFactors_):
    WeightBTagInterface(),
    bTagger(bTagger_), efficiencies(move(efficiencies_)), scaleFactors(move(scaleFactors_)),
    workingPoint(workingPoint_)
{}


WeightBTag::WeightBTag(shared_ptr<BTagger const> &bTagger_,
 unique_ptr<BTagEffInterface> &efficiencies_, unique_ptr<BTagSFInterface> &scaleFactors_):
    WeightBTag(bTagger_, bTagger_->GetWorkingPoint(), efficiencies_, scaleFactors_)
{}


WeightBTag::WeightBTag(WeightBTag const &src):
    WeightBTagInterface(src),
    bTagger(src.bTagger),  // Points to the same object
    efficiencies(src.efficiencies->Clone()),
    scaleFactors(src.scaleFactors->Clone()),
    workingPoint(src.workingPoint)
{}


WeightBTag::WeightBTag(WeightBTag &&src):
    WeightBTagInterface(move(src)),
    bTagger(move(src.bTagger)),
    efficiencies(move(src.efficiencies)),
    scaleFactors(move(src.scaleFactors)),
    workingPoint(src.workingPoint)
{}


WeightBTag::~WeightBTag()
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
        double const eff = efficiencies->GetEfficiency(workingPoint, jet);
        double const sf = scaleFactors->GetScaleFactor(workingPoint, jet,
         TranslateVariation(var, jet.GetParentID()));
        
        if (bTagger->IsTagged(workingPoint, jet))
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

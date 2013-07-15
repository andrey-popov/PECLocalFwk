#include <WeightBTag.hpp>


using namespace std;


WeightBTag::WeightBTag(BTagger const &bTagger_, BTagDatabase const &bTagDatabase_):
    bTagger(bTagger_), bTagDatabase(bTagDatabase_),
    defaultVar(Variation::Central)
{}


void WeightBTag::SetDefaultVariation(Variation var)
{
    defaultVar = var;
}


double WeightBTag::CalcWeight(vector<Jet> const &jets) const
{
    return CalcWeight(jets, defaultVar);
}


double WeightBTag::CalcWeight(vector<Jet> const &jets, Variation var) const
{
    // Logarithms of probabilities that the given tag configuration takes place with data-like or
    //MC-like b-tagging efficiencies. Since the probabilities for several jets will be multiplied,
    //it is more robust to operate with logarithms
    double logProbData = 0., logProbMC = 0.;
    
    
    // Loop over the jets
    for (auto const &jet: jets)
    {
        // Precalculate b-tagging efficiency and scale factor with the current jet
        double const eff = bTagDatabase.GetEfficiency(jet);
        double const sf = bTagDatabase.GetScaleFactor(jet,
         TranslateVariation(var, jet.GetParentID()));
        
        if (bTagger.IsTagged(jet))
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


BTagDatabase::SFVar WeightBTag::TranslateVariation(Variation var, int jetPDGID) const
{
    unsigned const absFlavour = abs(jetPDGID);
    
    if (absFlavour == 5 or absFlavour == 4)
    {
        if (var == Variation::TagRateUp)
            return BTagDatabase::SFVar::Up;
        else if (var == Variation::TagRateDown)
            return BTagDatabase::SFVar::Down;
    }
    else  // light-flavour and gluon jets
    {
        if (var == Variation::MistagRateUp)
            return BTagDatabase::SFVar::Up;
        else if (var == Variation::MistagRateDown)
            return BTagDatabase::SFVar::Down;
    }
    
    // If control has reached this point, it is either pure nominal or mismatched variation (like
    //requesting TagRateUp for a gluon jet)
    return BTagDatabase::SFVar::Central;
}

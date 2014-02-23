#include <WeightBTagInterface.hpp>


WeightBTagInterface::~WeightBTagInterface()
{}


void WeightBTagInterface::LoadPayload(Dataset const &)
{}


BTagSFInterface::Variation WeightBTagInterface::TranslateVariation(Variation var, int jetPDGID)
{
    unsigned const absFlavour = abs(jetPDGID);
    
    if (absFlavour == 5 or absFlavour == 4)
    {
        if (var == Variation::TagRateUp)
            return BTagSFInterface::Variation::Up;
        else if (var == Variation::TagRateDown)
            return BTagSFInterface::Variation::Down;
    }
    else  // light-flavour and gluon jets
    {
        if (var == Variation::MistagRateUp)
            return BTagSFInterface::Variation::Up;
        else if (var == Variation::MistagRateDown)
            return BTagSFInterface::Variation::Down;
    }
    
    // If control has reached this point, it is either pure nominal or mismatched variation (like
    //requesting TagRateUp for a gluon jet)
    return BTagSFInterface::Variation::Nominal;
}

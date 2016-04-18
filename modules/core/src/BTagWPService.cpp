#include <mensura/core/BTagWPService.hpp>

#include <stdexcept>
#include <sstream>


BTagWPService::BTagWPService(std::string name /*= "BTagWPService"*/):
    Service(name)
{
    // Set thresholds corresponding to official working points [1]
    //[1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation76X?rev=24#Supported_Algorithms_and_Operati
    SetThreshold({BTagger::Algorithm::CSV, BTagger::WorkingPoint::Loose}, 0.460);
    SetThreshold({BTagger::Algorithm::CSV, BTagger::WorkingPoint::Medium}, 0.800);
    SetThreshold({BTagger::Algorithm::CSV, BTagger::WorkingPoint::Tight}, 0.935);
    SetThreshold({BTagger::Algorithm::CMVA, BTagger::WorkingPoint::Loose}, -0.715);
    SetThreshold({BTagger::Algorithm::CMVA, BTagger::WorkingPoint::Medium}, 0.185);
    SetThreshold({BTagger::Algorithm::CMVA, BTagger::WorkingPoint::Tight}, 0.875);
}


BTagWPService::~BTagWPService() noexcept
{}


Service *BTagWPService::Clone() const
{
    return new BTagWPService(*this);
}


double BTagWPService::GetThreshold(BTagger const &tagger) const
{
    // Find the threshold for the given working point
    auto thresholdIt = thresholds.find(tagger);
    
    if (thresholdIt == thresholds.end())
    {
        std::ostringstream ost;
        ost << "BTagWPService::IsTagged: No threshold is available for b-tagger " <<
          tagger.GetTextCode() << ".";
        
        throw std::runtime_error(ost.str());
    }
    
    
    return thresholdIt->second;
}


bool BTagWPService::IsTagged(BTagger const &tagger, Jet const &jet) const
{
    // First, check the jet pseudorapidity makes sense
    if (fabs(jet.Eta()) > BTagger::GetMaxPseudorapidity())
        // There is a very small number of tagged jets with |eta| just above 2.4
        return false;
    
    
    // Compare discriminator value to the threshold
    return (jet.BTag(tagger.GetAlgorithm()) > GetThreshold(tagger));
}


void BTagWPService::SetThreshold(BTagger const &tagger, double threshold)
{
    thresholds[tagger] = threshold;
}

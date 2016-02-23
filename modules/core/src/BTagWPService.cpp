#include <PECFwk/core/BTagWPService.hpp>

#include <PECFwk/core/BTagSFInterface.hpp>

#include <stdexcept>
#include <sstream>


BTagWPService::BTagWPService(std::string name /*= "BTagWPService"*/):
    Service(name)
{
    // Set thresholds corresponding to official working points [1]
    //[1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/BtagRecommendation74X?rev=12
    SetThreshold({BTagger::Algorithm::CSV, BTagger::WorkingPoint::Loose}, 0.605);
    SetThreshold({BTagger::Algorithm::CSV, BTagger::WorkingPoint::Medium}, 0.890);
    SetThreshold({BTagger::Algorithm::CSV, BTagger::WorkingPoint::Tight}, 0.970);
    SetThreshold({BTagger::Algorithm::JP, BTagger::WorkingPoint::Loose}, 0.275);
    SetThreshold({BTagger::Algorithm::JP, BTagger::WorkingPoint::Medium}, 0.545);
    SetThreshold({BTagger::Algorithm::JP, BTagger::WorkingPoint::Tight}, 0.790);
}


BTagWPService::~BTagWPService() noexcept
{}


bool BTagWPService::IsTagged(BTagger const &tagger, Jet const &jet) const
{
    // First, check the jet pseudorapidity makes sense
    if (fabs(jet.Eta()) > BTagSFInterface::GetMaxPseudorapidity())
        // There is a very small number of tagged jets with |eta| just above 2.4
        return false;
    
    
    // Find the threshold for the given working point
    auto thresholdIt = thresholds.find(tagger);
    
    if (thresholdIt == thresholds.end())
    {
        std::ostringstream ost;
        ost << "BTagWPService::IsTagged: No threshold is available for b-tagger " <<
          tagger.GetTextCode() << ".";
        
        throw std::runtime_error(ost.str());
    }
    
    
    // Compare discriminator value with the threshold
    return (jet.BTag(tagger.GetAlgorithm()) > thresholdIt->second);
}


void BTagWPService::SetThreshold(BTagger const &tagger, double threshold)
{
    thresholds[tagger] = threshold;
}

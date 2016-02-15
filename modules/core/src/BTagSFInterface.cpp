#include <PECFwk/core/BTagSFInterface.hpp>


using namespace std;


// Definition of a static data member
double const BTagSFInterface::maxPseudorapidity = 2.4;


BTagSFInterface::BTagSFInterface() noexcept:
    defaultWP(BTagger::WorkingPoint::Tight)
{}


BTagSFInterface::~BTagSFInterface() noexcept
{}


double BTagSFInterface::GetScaleFactor(BTagger::WorkingPoint wp, Jet const &jet,
 Variation var /*= Variation::Nominal*/) const
{
    return GetScaleFactor(wp, jet, jet.GetParentID(), var);
}


double BTagSFInterface::GetScaleFactor(Candidate const &jet, int flavour,
 Variation var /*= Variation::Nominal*/) const
{
    return GetScaleFactor(defaultWP, jet, flavour, var);
}


double BTagSFInterface::GetScaleFactor(Jet const &jet, Variation var /*= Variation::Nominal*/) const
{
    return GetScaleFactor(defaultWP, jet, jet.GetParentID(), var);
}


void BTagSFInterface::SetDefaultWorkingPoint(BTagger::WorkingPoint wp)
{
    defaultWP = wp;
}


double BTagSFInterface::GetMaxPseudorapidity()
{
    return maxPseudorapidity;
}

#include <BTagSFInterface.hpp>


using namespace std;


// Definition of a static data member
double const BTagSFInterface::maxPseudorapidity = 2.4;


BTagSFInterface::BTagSFInterface():
    defaultWP(BTagger::WorkingPoint::Tight)
{}


double BTagSFInterface::GetScaleFactor(Jet const &jet, Variation var /*= Variation::Nominal*/) const
{
    return GetScaleFactor(defaultWP, jet, var);
}


void BTagSFInterface::SetDefaultWorkingPoint(BTagger::WorkingPoint wp)
{
    defaultWP = wp;
}


double BTagSFInterface::GetMaxPseudorapidity()
{
    return maxPseudorapidity;
}

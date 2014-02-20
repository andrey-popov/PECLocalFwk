#include <BTagSFInterface.hpp>


using namespace std;


BTagSFInterface::BTagSFInterface():
    defaultWP(BTagger::WorkingPoint::Tight)
{}


double BTagSFInterface::GetScaleFactor(Jet const &jet, Variation var /*= Variation::Central*/) const
{
    return GetScaleFactor(defaultWP, jet, var);
}


void BTagSFInterface::SetDefaultWorkingPoint(BTagger::WorkingPoint wp)
{
    defaultWP = wp;
}

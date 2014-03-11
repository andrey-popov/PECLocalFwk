#include <BTagEffInterface.hpp>


BTagEffInterface::BTagEffInterface():
    defaultWP(BTagger::WorkingPoint::Tight)
{}


BTagEffInterface::~BTagEffInterface()
{}


void BTagEffInterface::LoadPayload(Dataset const &)
{}


double BTagEffInterface::GetEfficiency(BTagger::WorkingPoint wp, Jet const &jet) const
{
    return GetEfficiency(wp, jet, jet.GetParentID());
}


double BTagEffInterface::GetEfficiency(Candidate const &jet, int flavour) const
{
    return GetEfficiency(defaultWP, jet, flavour);
}


double BTagEffInterface::GetEfficiency(Jet const &jet) const
{
    return GetEfficiency(defaultWP, jet, jet.GetParentID());
}


void BTagEffInterface::SetDefaultWorkingPoint(BTagger::WorkingPoint wp)
{
    defaultWP = wp;
}

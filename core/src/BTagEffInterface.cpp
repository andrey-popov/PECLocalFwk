#include <BTagEffInterface.hpp>


BTagEffInterface::BTagEffInterface():
    defaultWP(BTagger::WorkingPoint::Tight)
{}


void BTagEffInterface::LoadPayload(Dataset const &)
{}


double BTagEffInterface::GetEfficiency(Jet const &jet) const
{
    return GetEfficiency(defaultWP, jet);
}


void BTagEffInterface::SetDefaultWorkingPoint(BTagger::WorkingPoint wp)
{
    defaultWP = wp;
}

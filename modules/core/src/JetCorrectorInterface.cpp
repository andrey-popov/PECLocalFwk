#include <mensura/core/JetCorrectorInterface.hpp>


void JetCorrectorInterface::Init()
{}


void JetCorrectorInterface::operator()(Jet &jet, double rho,
 SystVariation syst /*= SystVariation()*/) const
{
    Correct(jet, rho, syst);
}

#include <JetCorrectorInterface.hpp>


void JetCorrectorInterface::Init()
{}


void JetCorrectorInterface::operator()(Jet &jet, double rho,
 SystVariation syst /*= SystVariation()*/)
{
    Correct(jet, rho, syst);
}

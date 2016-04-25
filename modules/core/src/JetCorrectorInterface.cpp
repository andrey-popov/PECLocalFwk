#include <mensura/core/JetCorrectorInterface.hpp>


void JetCorrectorInterface::Init()
{}


void JetCorrectorInterface::operator()(Jet &jet, double rho, SystType syst /*= SystType::None*/,
  SystService::VarDirection direction /*= SystService::VarDirection::Undefined*/) const
{
    Correct(jet, rho, syst, direction);
}

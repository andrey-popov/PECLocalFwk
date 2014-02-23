#include <EventSelectionInterface.hpp>


EventSelectionInterface::~EventSelectionInterface()
{}


bool EventSelectionInterface::IsAnalysisJet(Jet const &) const
{
    return true;
}
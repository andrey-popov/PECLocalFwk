#include <TriggerSelectionInterface.hpp>


TriggerSelectionInterface::TriggerSelectionInterface():
    triggerTree(nulptr),
    nEntriesTree(0), nextEntryTree(0)
{}


TriggerSelectionInterface::TriggerSelectionInterface(TriggerSelectionInterface const &src):
    triggerTree(src.triggerTree),
    nEntriesTree(src.nEntriesTree), nextEntryTree(src.nextEntryTree)
{}


TriggerSelectionInterface &TriggerSelectionInterface::operator=(
 TriggerSelectionInterface const &rhs)
{
    if (this != &rhs)
    {
        triggerTree = rhs.triggerTree;
        nEntriesTree = rhs.nEntriesTree;
        nextEntryTree = rhs.nextEntryTree;
    }
    
    return *this;
}

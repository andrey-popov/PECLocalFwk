#include <TriggerSelectionInterface.hpp>


TriggerSelectionInterface::TriggerSelectionInterface():
    triggerTree(nulptr),
    nEntriesTree(0), nextEntryTree(0)
{}


TriggerSelectionInterface::TriggerSelectionInterface(TriggerSelectionInterface &&src):
    triggerTree(src.triggerTree),
    nEntriesTree(src.nEntriesTree), nextEntryTree(src.nextEntryTree)
{
    src.triggerTree = nulptr;
    src.nEntriesTree = 0;
    src.nextEntryTree = 0;
}


TriggerSelectionInterface &TriggerSelectionInterface::operator=(TriggerSelectionInterface &&rhs)
{
    if (this != &rhs)
    {
        triggerTree = rhs.triggerTree;
        nEntriesTree = rhs.nEntriesTree;
        nextEntryTree = rhs.nextEntryTree;
        
        rhs.triggerTree = nulptr;
        rhs.nEntriesTree = 0;
        rhs.nextEntryTree = 0;
    }
    
    return *this;
}

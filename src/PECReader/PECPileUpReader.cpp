#include <mensura/PECReader/PECPileUpReader.hpp>

#include <mensura/Processor.hpp>
#include <mensura/ROOTLock.hpp>
#include <mensura/PECReader/PECInputData.hpp>

#include "PileUpInfo.hpp"


PECPileUpReader::PECPileUpReader(std::string const name /*= "PileUp"*/):
    PileUpReader(name),
    inputDataPluginName("InputData"),
    inputDataPlugin(nullptr),
    treeName("pecPileUp/PileUp"), bfPileUpInfoP(nullptr)
{}


PECPileUpReader::PECPileUpReader(PECPileUpReader const &src) noexcept:
    PileUpReader(src),
    inputDataPluginName(src.inputDataPluginName),
    inputDataPlugin(src.inputDataPlugin),
    treeName(src.treeName),
    bfPileUpInfoP(nullptr)
{}


PECPileUpReader::~PECPileUpReader() noexcept
{}


void PECPileUpReader::BeginRun(Dataset const &)
{
    // Save pointer to the plugin providing access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(GetDependencyPlugin(inputDataPluginName));
    
    
    // Set up the tree with pile-up information. Attributes of the PileUpInfo class that are not
    //used are not read
    inputDataPlugin->LoadTree(treeName);
    TTree *tree = inputDataPlugin->ExposeTree(treeName);
    ROOTLock::Lock();
    tree->SetBranchStatus("inTimeNumPU", false);
    tree->SetBranchStatus("maxPtHat", false);
    tree->SetBranchAddress("puInfo", &bfPileUpInfoP);
    ROOTLock::Unlock();
}


Plugin *PECPileUpReader::Clone() const
{
    return new PECPileUpReader(*this);
}


bool PECPileUpReader::ProcessEvent()
{
    // Read the tree
    inputDataPlugin->ReadEventFromTree(treeName);
    
    numVertices = bfPileUpInfoP->NumPV();
    expectedPileUp = bfPileUpInfoP->TrueNumPU();
    rho = bfPileUpInfoP->Rho();
    
    
    // Since this reader does not have access to the input file, it does not know when there are
    //no more events in the dataset and thus always returns true
    return true;
}

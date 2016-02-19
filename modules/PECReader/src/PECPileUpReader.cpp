#include <PECFwk/PECReader/PECPileUpReader.hpp>

#include <PECFwk/core/Processor.hpp>
#include <PECFwk/PECReader/PECInputData.hpp>


PECPileUpReader::PECPileUpReader(std::string const name /*= "PileUp"*/):
    PileUpReader(name),
    inputDataPluginName("InputData"),
    inputDataPlugin(nullptr),
    treeName("pecPileUp/PileUp"), bfPileUpInfoPointer(&bfPileUpInfo)
{}


PECPileUpReader::~PECPileUpReader()
{}


void PECPileUpReader::BeginRun(Dataset const &)
{
    // Save pointer to the plugin providing access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(
      GetMaster().GetPluginBefore(inputDataPluginName, GetName()));
    
    
    // Set up the tree with pile-up information. Attributes of the PileUpInfo class that are not
    //used are not read
    inputDataPlugin->LoadTree(treeName);
    TTree *tree = inputDataPlugin->ExposeTree(treeName);
    tree->SetBranchStatus("puInfo.inTimePU", false);
    tree->SetBranchAddress("puInfo", &bfPileUpInfoPointer);
}


Plugin *PECPileUpReader::Clone() const
{
    return new PECPileUpReader(GetName());
}


bool PECPileUpReader::ProcessEvent()
{
    // Read the tree
    inputDataPlugin->ReadEventFromTree(treeName);
    
    numVertices = bfPileUpInfo.NumPV();
    expectedPileUp = bfPileUpInfo.TrueNumPU();
    rho = bfPileUpInfo.Rho();
    
    
    // Since this reader does not have access to the input file, it does not know when there are
    //no more events in the dataset and thus always returns true
    return true;
}

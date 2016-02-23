#include <PECFwk/PECReader/PECGeneratorReader.hpp>

#include <PECFwk/core/Processor.hpp>
#include <PECFwk/PECReader/PECInputData.hpp>

#include <stdexcept>


using namespace std::literals::string_literals;


PECGeneratorReader::PECGeneratorReader(std::string const name /*= "Generator"*/):
    ReaderPlugin(name),
    inputDataPluginName("InputData"),
    inputDataPlugin(nullptr),
    treeName("pecGenerator/Generator"), bfGeneratorPointer(&bfGenerator)
{}


PECGeneratorReader::~PECGeneratorReader()
{}


void PECGeneratorReader::BeginRun(Dataset const &dataset)
{
    // Throw an exception if the plugin is run on data
    if (not dataset.IsMC())
        throw std::logic_error("PECGeneratorReader::BeginRun: Plugin \""s + GetName() +
          "\" is requested to process data while it can only run on simulation.");
    
    
    // Save pointer to the plugin providing access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(
      GetMaster().GetPluginBefore(inputDataPluginName, GetName()));
    
    
    // Set up the tree with generator information. Only some attributes of the PEC object are read
    inputDataPlugin->LoadTree(treeName);
    TTree *tree = inputDataPlugin->ExposeTree(treeName);
    
    tree->SetBranchStatus("*", false);
    tree->SetBranchStatus("processId", true);
    tree->SetBranchStatus("nominalWeight", true);
    
    tree->SetBranchAddress("generator", &bfGeneratorPointer);
}


Plugin *PECGeneratorReader::Clone() const
{
    return new PECGeneratorReader(GetName());
}


double PECGeneratorReader::GetNominalWeight() const
{
    return bfGenerator.NominalWeight();
}


int PECGeneratorReader::GetProcessID() const
{
    return bfGenerator.ProcessId();
}


bool PECGeneratorReader::ProcessEvent()
{
    // Read the tree
    inputDataPlugin->ReadEventFromTree(treeName);
    
    
    // Since this reader does not have access to the input file, it does not know when there are
    //no more events in the dataset and thus always returns true
    return true;
}

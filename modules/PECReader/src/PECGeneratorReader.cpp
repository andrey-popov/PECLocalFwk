#include <mensura/PECReader/PECGeneratorReader.hpp>

#include <mensura/Processor.hpp>
#include <mensura/ROOTLock.hpp>
#include <mensura/PECReader/PECInputData.hpp>

#include <sstream>
#include <stdexcept>


using namespace std::literals::string_literals;


PECGeneratorReader::PECGeneratorReader(std::string const name /*= "Generator"*/):
    GeneratorReader(name),
    inputDataPluginName("InputData"),
    inputDataPlugin(nullptr),
    readAltWeights(false),
    treeName("pecGenerator/Generator"), bfGeneratorPointer(&bfGenerator)
{}


PECGeneratorReader::PECGeneratorReader(PECGeneratorReader const &src) noexcept:
    GeneratorReader(src),
    inputDataPluginName(src.inputDataPluginName),
    inputDataPlugin(src.inputDataPlugin),
    readAltWeights(src.readAltWeights),
    treeName(src.treeName),
    bfGeneratorPointer(&bfGenerator)
{}


void PECGeneratorReader::BeginRun(Dataset const &dataset)
{
    // Throw an exception if the plugin is run on data
    if (not dataset.IsMC())
    {
        std::ostringstream message;
        message << "PECGeneratorReader[\"" << GetName() << "\"]::BeginRun: " <<
          "Attempt to run over data while this plugin can only run on simulated events.";
        throw std::logic_error(message.str());
    }
    
    
    // Save pointer to the plugin providing access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(GetDependencyPlugin(inputDataPluginName));
    
    
    // Set up the tree with generator information. Only some attributes of the PEC object are read
    inputDataPlugin->LoadTree(treeName);
    TTree *tree = inputDataPlugin->ExposeTree(treeName);
    
    ROOTLock::Lock();
    if (not readAltWeights)
        tree->SetBranchStatus("altWeights", false);
    
    tree->SetBranchAddress("generator", &bfGeneratorPointer);
    ROOTLock::Unlock();
}


Plugin *PECGeneratorReader::Clone() const
{
    return new PECGeneratorReader(*this);
}


double PECGeneratorReader::GetAltWeight(unsigned index) const
{
    if (not readAltWeights)
    {
        std::ostringstream message;
        message << "PECGeneratorReader[\"" << GetName() <<
          "\"]::GetAltWeight: Reading of alternative weights has not been requested.";
        throw std::logic_error(message.str());
    }
    
    if (index >= bfGenerator.AltWeights().size())
    {
        std::ostringstream message;
        message << "PECGeneratorReader[\"" << GetName() <<
          "\"]::GetAltWeight: Weight with index " << index << " is requested, but only " <<
          bfGenerator.AltWeights().size() << " weights are available.";
        throw std::logic_error(message.str());
    }
    
    return bfGenerator.AltWeights()[index];
}


double PECGeneratorReader::GetNominalWeight() const
{
    return bfGenerator.NominalWeight();
}


unsigned PECGeneratorReader::GetNumAltWeights() const
{
    if (not readAltWeights)
        return 0;
    else
        return bfGenerator.AltWeights().size();
}


std::pair<int, int> PECGeneratorReader::GetPdfPart() const
{
    return {bfGenerator.PdfId(0), bfGenerator.PdfId(1)};
}


std::pair<double, double> PECGeneratorReader::GetPdfX() const
{
    return {bfGenerator.PdfX(0), bfGenerator.PdfX(1)};
}


int PECGeneratorReader::GetProcessID() const
{
    return bfGenerator.ProcessId();
}


double PECGeneratorReader::GetScale() const
{
    return bfGenerator.PdfQScale();
}


void PECGeneratorReader::RequestAltWeights(bool on /*= true*/)
{
    readAltWeights = on;
}


bool PECGeneratorReader::ProcessEvent()
{
    // Read the tree
    inputDataPlugin->ReadEventFromTree(treeName);
    
    
    // Since this reader does not have access to the input file, it does not know when there are
    //no more events in the dataset and thus always returns true
    return true;
}

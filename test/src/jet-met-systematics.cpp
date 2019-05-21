/**
 * This test program applies requested JERC variations and prints resulting jet pt and MET.
 */

#include <mensura/Dataset.hpp>
#include <mensura/Processor.hpp>
#include <mensura/SystService.hpp>

#include <mensura/PECReader/PECInputData.hpp>
#include <mensura/PECReader/PECJetMETReader.hpp>

#include <iostream>
#include <memory>


using namespace std;


int main(int argc, char **argv)
{
    // Parse arguments to deduce requested systematic variation
    string systType("None");
    SystService::VarDirection systDirection = SystService::VarDirection::Undefined;
    
    if (argc == 2)
    {
        string const argText(argv[1]);
        
        if (argText != "None")
        {
            cerr << "When one argument is provided, the only allowed value is \"None\" while \"" <<
              argText << "\" is given.\n";
            return EXIT_FAILURE;
        }
    }
    else if (argc == 3)
    {
        string const arg1Text(argv[1]);
        string const arg2Text(argv[2]);
        
        if (arg1Text != "JEC" and arg1Text != "JER" and arg1Text != "METUncl")
        {
            cerr << "Cannot recognize argument \"" << arg1Text << "\".\n";
            return EXIT_FAILURE;
        }
        else
            systType = arg1Text;
        
        if (arg2Text == "up")
            systDirection = SystService::VarDirection::Up;
        else if (arg2Text == "down")
            systDirection = SystService::VarDirection::Down;
        else
        {
            cerr << "Cannot recognize argument \"" << arg2Text << "\".\n";
            return EXIT_FAILURE;
        }
    }
    
    
    // Input dataset
    Dataset dataset(Dataset::Type::MC);
    dataset.AddFile("../ttbar.root");
    dataset.SetNormalization(831.76, 1000000 /* a dummy value */);
    
    
    // Processor object
    Processor processor;
    
    
    // Register a service to declare systematic variation
    processor.RegisterService(new SystService(systType, systDirection));
    
    
    // Register plugins
    processor.RegisterPlugin(new PECInputData);
    
    PECJetMETReader *jetmetReader = new PECJetMETReader;
    jetmetReader->ConfigureLeptonCleaning("");  // Disabled
    processor.RegisterPlugin(jetmetReader);
    
    
    // Save pointers to selected plugins to read information from them in the event loop
    PECInputData const *inputData =
      dynamic_cast<PECInputData const *>(processor.GetPlugin("InputData"));
    
    
    // Open the input dataset
    processor.OpenDataset(dataset);
    
    
    // Loop over few events
    unsigned const maxEventsToPrint = 5;
    unsigned nEventsPrinted = 0;
    
    while (true)
    {
        // Process a new event from the dataset
        Plugin::EventOutcome const status = processor.ProcessEvent();
        
        // Skip to the next event if the current one has been rejected by a filter
        if (status == Plugin::EventOutcome::FilterFailed)
            continue;
        
        // Terminate the loop if there are no events left in the dataset
        if (status == Plugin::EventOutcome::NoEvents)
            break;
        
        
        // Print out some properties of jets and MET
        auto const &eventID = inputData->GetEventID();
        cout << "\033[0;34m***** Event " << eventID.Run() << ":" << eventID.LumiBlock() << ":" <<
          eventID.Event() << " *****\033[0m\n";
        
        cout << "Jet pt:";
        
        for (auto const &j: jetmetReader->GetJets())
            cout << " " << j.Pt();
        
        cout << "\nMET: " << jetmetReader->GetMET().Pt() << "\n\n";
        
        
        ++nEventsPrinted;
        
        if (nEventsPrinted == maxEventsToPrint)
            break;
    }
    
    
    return EXIT_SUCCESS;
}

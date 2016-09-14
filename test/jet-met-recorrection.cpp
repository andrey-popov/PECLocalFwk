/**
 * This test program reapplies jet corrections, propagates them into MET via type-1 corrections,
 * and compares results to the reference.
 */

#include <mensura/core/Dataset.hpp>
#include <mensura/core/Processor.hpp>
#include <mensura/core/SystService.hpp>

#include <mensura/extensions/JetCorrectorService.hpp>
#include <mensura/extensions/JetMETUpdate.hpp>

#include <mensura/PECReader/PECGenJetMETReader.hpp>
#include <mensura/PECReader/PECInputData.hpp>
#include <mensura/PECReader/PECJetMETReader.hpp>
#include <mensura/PECReader/PECPileUpReader.hpp>

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
    dataset.AddFile("ttbar.root", 831.76, 1000000 /* a dummy value */);
    
    
    // Processor object
    Processor processor;
    
    
    // Register a service to declare systematic variation
    processor.RegisterService(new SystService(systType, systDirection));
    
    // Register jet corrector services
    JetCorrectorService *jetCorrFull = new JetCorrectorService("JetCorrFull");
    jetCorrFull->SetJEC({"Fall15_25nsV2_MC_L1FastJet_AK4PFchs.txt",
      "Fall15_25nsV2_MC_L2Relative_AK4PFchs.txt", "Fall15_25nsV2_MC_L3Absolute_AK4PFchs.txt"});
    jetCorrFull->SetJECUncertainty("Fall15_25nsV2_MC_Uncertainty_AK4PFchs.txt");
    jetCorrFull->SetJER("Fall15_25nsV2_MC_JERSF_AK4PFchs.txt",
      "Fall15_25nsV2_MC_PtResolution_AK4PFchs.txt");
    processor.RegisterService(jetCorrFull);
    
    JetCorrectorService *jetCorrL123 = new JetCorrectorService("jetCorrL123");
    jetCorrL123->SetJEC({"Fall15_25nsV2_MC_L1FastJet_AK4PFchs.txt",
      "Fall15_25nsV2_MC_L2Relative_AK4PFchs.txt", "Fall15_25nsV2_MC_L3Absolute_AK4PFchs.txt"});
    jetCorrL123->SetJECUncertainty("Fall15_25nsV2_MC_Uncertainty_AK4PFchs.txt");
    processor.RegisterService(jetCorrL123);
    
    JetCorrectorService *jetCorrL123Undo = new JetCorrectorService("jetCorrL123Undo");
    jetCorrL123Undo->SetJEC({"Fall15_25nsV2_MC_L1FastJet_AK4PFchs.txt",
      "Fall15_25nsV2_MC_L2Relative_AK4PFchs.txt", "Fall15_25nsV2_MC_L3Absolute_AK4PFchs.txt"});
    jetCorrL123Undo->SetJECUncertainty("Fall15_25nsV2_MC_Uncertainty_AK4PFchs.txt");
    processor.RegisterService(jetCorrL123Undo);
    
    
    // Register plugins
    processor.RegisterPlugin(new PECInputData);
    processor.RegisterPlugin(new PECPileUpReader);
    processor.RegisterPlugin(new PECGenJetMETReader);
    
    PECJetMETReader *jetmetReader = new PECJetMETReader("OrigJetMET");
    jetmetReader->ConfigureLeptonCleaning("");  // Disabled
    jetmetReader->SetGenJetReader();  // Default one
    processor.RegisterPlugin(jetmetReader);
    
    JetMETUpdate *jetmetUpdater = new JetMETUpdate;
    jetmetUpdater->SetJetCorrection("JetCorrFull");
    jetmetUpdater->SetJetCorrectionForMET("jetCorrL123", "", "jetCorrL123Undo", "");
    processor.RegisterPlugin(jetmetUpdater);
    
    
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
        
        cout << "Original jet pt:";
        
        for (auto const &j: jetmetReader->GetJets())
            cout << " " << j.Pt();
        
        cout << "\nOriginal MET: " << jetmetReader->GetMET().Pt() << '\n';
        
        cout << "Updated jet pt: ";
        
        for (auto const &j: jetmetUpdater->GetJets())
            cout << " " << j.Pt();
        
        cout << "\nUpdated MET:  " << jetmetUpdater->GetMET().Pt() << "\n\n";
        
        
        ++nEventsPrinted;
        
        if (nEventsPrinted == maxEventsToPrint)
            break;
    }
    
    
    return EXIT_SUCCESS;
}

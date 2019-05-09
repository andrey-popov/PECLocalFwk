/**
 * This program tests JetCorrectorService. Corrections are evaluated in a stand-alone manner, and
 * momenta of jets are not altered.
 */

#include <mensura/Dataset.hpp>
#include <mensura/JetCorrectorService.hpp>
#include <mensura/Processor.hpp>

#include <mensura/PECReader/PECGenJetMETReader.hpp>
#include <mensura/PECReader/PECInputData.hpp>
#include <mensura/PECReader/PECJetMETReader.hpp>
#include <mensura/PECReader/PECPileUpReader.hpp>

#include <iostream>
#include <memory>


using namespace std;


int main()
{
    // Input dataset
    Dataset dataset(Dataset::Type::MC);
    dataset.AddFile("../ttbar.root", 831.76, 1000000 /* a dummy value */);
    
    
    // Processor object
    Processor processor;
    
    
    // Register jet corrector service
    JetCorrectorService *jetCorrector = new JetCorrectorService;
    jetCorrector->SetJEC({"Fall15_25nsV2_MC_L1FastJet_AK4PFchs.txt",
      "Fall15_25nsV2_MC_L2Relative_AK4PFchs.txt", "Fall15_25nsV2_MC_L3Absolute_AK4PFchs.txt"});
    jetCorrector->SetJECUncertainty("Fall15_25nsV2_MC_Uncertainty_AK4PFchs.txt");
    jetCorrector->SetJER("Fall15_25nsV2_MC_JERSF_AK4PFchs.txt",
      "Fall15_25nsV2_MC_PtResolution_AK4PFchs.txt");
    processor.RegisterService(jetCorrector);
    
    
    // Register plugins
    processor.RegisterPlugin(new PECInputData);
    processor.RegisterPlugin(new PECGenJetMETReader);
    
    PECJetMETReader *jetmetReader = new PECJetMETReader;
    jetmetReader->ConfigureLeptonCleaning("");  // Disabled
    jetmetReader->SetGenJetReader();  // Default one
    processor.RegisterPlugin(jetmetReader);
    
    processor.RegisterPlugin(new PECPileUpReader);
    
    
    // Save pointers to selected plugins to read information from them in the event loop
    PECInputData const *inputData =
      dynamic_cast<PECInputData const *>(processor.GetPlugin("InputData"));
    
    PileUpReader const *puReader =
      dynamic_cast<PileUpReader const *>(processor.GetPlugin("PileUp"));
    
    
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
        
        
        double const rho = puReader->GetRho();
        
        
        cout << "Jets\n";
        unsigned curJetNumber = 0;
        
        for (auto const &j: jetmetReader->GetJets())
        {
            cout << " #" << ++curJetNumber << ":\n";
            
            double const rawPt = j.RawP4().Pt();
            cout << "  Raw pt: " << rawPt << ", corrected pt out of the box: " << j.Pt() << '\n';
            
            double const corrFactor = jetCorrector->Eval(j, rho);
            cout << "  Correction factor: " << corrFactor << '\n';
            cout << "  JEC uncertainty: " <<
              jetCorrector->EvalJECUnc(rawPt * corrFactor, j.Eta()) << '\n';
            cout << "  Has GEN-level match: " << bool(j.MatchedGenJet()) << '\n';
            cout << "  Recorrected pt: " << rawPt * corrFactor << '\n';
        }
        
        cout << "\n\n";
        
        
        ++nEventsPrinted;
        
        if (nEventsPrinted == maxEventsToPrint)
            break;
    }
    
    
    return EXIT_SUCCESS;
}

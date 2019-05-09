/**
 * This test program runs over view events in the requested channel and reports computed lepton
 * scale factors.
 */

#include <mensura/Dataset.hpp>
#include <mensura/LeptonFilter.hpp>
#include <mensura/LeptonSFWeight.hpp>
#include <mensura/Processor.hpp>

#include <mensura/PECReader/PECInputData.hpp>
#include <mensura/PECReader/PECLeptonReader.hpp>

#include <iostream>
#include <memory>


using namespace std;


int main(int argc, char **argv)
{
    // Check what lepton flavour is selected
    Lepton::Flavour targetFlavour = Lepton::Flavour::Muon;
    
    if (argc == 2)
    {
        string const leptonText(argv[1]);
        
        if (leptonText == "e")
            targetFlavour = Lepton::Flavour::Electron;
        else if (leptonText != "mu")
        {
            cerr << "Usage: lepton-scale-factors [mu/e]\n";
            return EXIT_FAILURE;
        }
    }
    else if (argc > 2)
    {
        cerr << "Usage: lepton-scale-factors [mu/e]\n";
        return EXIT_FAILURE;
    }
    
    
    // Input dataset
    Dataset dataset(Dataset::Type::MC);
    dataset.AddFile("ttbar.root", 831.76, 1000000 /* a dummy value */);
    
    
    // Processor object
    Processor processor;
    
    
    // Register plugins
    processor.RegisterPlugin(new PECInputData);
    processor.RegisterPlugin(new PECLeptonReader);
    processor.RegisterPlugin(new LeptonFilter("LeptonFilter", targetFlavour, 30., 2.1));
    
    if (targetFlavour == Lepton::Flavour::Muon)
        processor.RegisterPlugin(new LeptonSFWeight(Lepton::Flavour::Muon,
          "MuonSF_2016BCD_80Xv2.root", {"ID_Tight"}));
    else
        processor.RegisterPlugin(new LeptonSFWeight(Lepton::Flavour::Electron,
          "ElectronSF_2016BCD_80Xv2.root", {"CutBasedID_Tight"}));
    
    
    // Save pointers to selected plugins to read information from them in the event loop
    LeptonReader const *leptonReader =
      dynamic_cast<LeptonReader const *>(processor.GetPlugin("Leptons"));
    LeptonSFWeight const *leptonSFReweighter =
      dynamic_cast<LeptonSFWeight const *>(processor.GetPlugin("LeptonSFWeight"));
    
    
    // Open the input dataset
    processor.OpenDataset(dataset);
    
    
    // Loop over few events
    unsigned const maxEventsToPrint = 10;
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
        
        
        // Print out information about leptons and the event weight based on their scale factors
        cout << "Leptons:\n";
        
        for (auto const &l: leptonReader->GetLeptons())
            cout << " flavour: " << int(l.GetFlavour()) << ", pt: " << l.Pt() << ", eta: " <<
              l.Eta() << '\n';
        
        cout << "Event weight: " << leptonSFReweighter->GetWeight() << "\n\n";
        
        
        ++nEventsPrinted;
        
        if (nEventsPrinted == maxEventsToPrint)
            break;
    }
    
    
    return EXIT_SUCCESS;
}

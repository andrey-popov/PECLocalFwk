#include <PECFwk/core/BTagWPService.hpp>
#include <PECFwk/core/Dataset.hpp>
#include <PECFwk/core/Processor.hpp>

#include <PECFwk/extensions/BTagEffService.hpp>
#include <PECFwk/extensions/BTagSFService.hpp>
#include <PECFwk/extensions/BTagWeight.hpp>
#include <PECFwk/extensions/JetFilter.hpp>
#include <PECFwk/extensions/LeptonFilter.hpp>
#include <PECFwk/extensions/MetFilter.hpp>

#include <PECFwk/PECReader/PECGeneratorReader.hpp>
#include <PECFwk/PECReader/PECGenParticleReader.hpp>
#include <PECFwk/PECReader/PECInputData.hpp>
#include <PECFwk/PECReader/PECJetMETReader.hpp>
#include <PECFwk/PECReader/PECLeptonReader.hpp>
#include <PECFwk/PECReader/PECPileUpReader.hpp>
#include <PECFwk/PECReader/PECTriggerFilter.hpp>

#include <iostream>
#include <memory>


using namespace std;


int main()
{
    // Input dataset
    Dataset dataset({Dataset::Process::ttbar}, Dataset::Generator::POWHEG);
    string const filePrefix("/gridgroup/cms/popov/PECData/2015Bravo/");
    dataset.AddFile(filePrefix + "ttbar-pw_3.0.0_VmF_p1.root", 831.76, 96834559);
    dataset.AddFile(filePrefix + "ttbar-pw_3.0.0_VmF_p2.root", 831.76, 96834559);
    dataset.AddFile(filePrefix + "ttbar-pw_3.0.0_VmF_p3.root", 831.76, 96834559);
    dataset.AddFile(filePrefix + "ttbar-pw_3.0.0_VmF_p4.root", 831.76, 96834559);
    dataset.AddFile(filePrefix + "ttbar-pw_3.0.0_VmF_p5.root", 831.76, 96834559);
    //^ Only a fraction of available files included here
    
    
    // Triggers
    list<TriggerRange> triggerRanges;
    triggerRanges.emplace_back(0, -1, "IsoMu20", 2244.966, "IsoMu20");
    
    
    // Common definition of b-tagging that will be used everywhere
    BTagger const bTagger(BTagger::Algorithm::CSV, BTagger::WorkingPoint::Tight);
    
    
    // Processor object
    Processor processor;
    
    
    // Register b-tagging services
    processor.RegisterService(new BTagWPService);
    
    BTagEffService *bTagEffService = new BTagEffService("BTagEff_74X_v1.0.root");
    bTagEffService->SetDefaultProcessLabel("ttbar");
    processor.RegisterService(bTagEffService);
    
    BTagSFService *bTagSFService = new BTagSFService(bTagger, "BTagSF_74X_CSVv2.csv");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Bottom, "mujets");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Charm, "mujets");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Light, "comb");
    processor.RegisterService(bTagSFService);
    
    
    // Register plugins
    processor.RegisterPlugin(new PECInputData);
    processor.RegisterPlugin(BuildPECTriggerFilter(false, triggerRanges));
    processor.RegisterPlugin(new PECLeptonReader);
    processor.RegisterPlugin(new LeptonFilter("LeptonFilter", Lepton::Flavour::Muon, 22., 2.1));
    
    PECJetMETReader *jetReader = new PECJetMETReader;
    jetReader->SetSelection(30., 2.4);
    processor.RegisterPlugin(jetReader);
    
    JetFilter *jetFilter = new JetFilter(0., bTagger);
    jetFilter->AddSelectionBin(4, -1, 2, 2);
    processor.RegisterPlugin(jetFilter);
    
    processor.RegisterPlugin(new MetFilter(MetFilter::Mode::MtW, 40.));
    processor.RegisterPlugin(new PECPileUpReader);
    processor.RegisterPlugin(new PECGeneratorReader);
    processor.RegisterPlugin(new PECGenParticleReader);
    processor.RegisterPlugin(new BTagWeight(bTagger));
    
    
    // Save pointers to selected plugins to read information from them in the event loop
    PECInputData const *inputData =
      dynamic_cast<PECInputData const *>(processor.GetPlugin("InputData"));
    LeptonReader const *leptonReader =
      dynamic_cast<LeptonReader const *>(processor.GetPlugin("Leptons"));
    PileUpReader const *puReader =
      dynamic_cast<PileUpReader const *>(processor.GetPlugin("PileUp"));
    PECGeneratorReader const *generatorReader =
      dynamic_cast<PECGeneratorReader const *>(processor.GetPlugin("Generator"));
    GenParticleReader const *genParticleReader =
      dynamic_cast<GenParticleReader const *>(processor.GetPlugin("GenParticles"));
    BTagWeight const *bTagReweighter =
      dynamic_cast<BTagWeight const *>(processor.GetPlugin("BTagWeight"));
    
    
    // Open the input dataset
    processor.OpenDataset(dataset);
    
    
    // Loop over few events
    for (unsigned i = 0; i < 500; ++i)
    {
        // Process a new event from the dataset
        Plugin::EventOutcome const status = processor.ProcessEvent();
        
        // Skip to the next event if the current one has been rejected by a filter
        if (status == Plugin::EventOutcome::FilterFailed)
            continue;
        
        // Terminate the loop if there are no events left in the dataset
        if (status == Plugin::EventOutcome::NoEvents)
            break;
        
        
        // Print out some information about the selected event
        auto const &eventID = inputData->GetEventID();
        cout << "\033[0;34m***** Event " << eventID.Run() << ":" << eventID.LumiBlock() << ":" <<
          eventID.Event() << " *****\033[0m\n";
        
        cout << "\nTight leptons:\n";
        
        for (auto const &l: leptonReader->GetLeptons())
            cout << " flavour: " << int(l.GetFlavour()) << ", pt: " << l.Pt() << ", eta: " <<
              l.Eta() << ", iso: " << l.RelIso() << '\n';
        
        cout << "\nLoose leptons:\n";
        
        for (auto const &l: leptonReader->GetLooseLeptons())
            cout << " flavour: " << int(l.GetFlavour()) << ", pt: " << l.Pt() << ", eta: " <<
              l.Eta() << ", iso: " << l.RelIso() << '\n';
        
        cout << "\nAnalysis jets:\n";
        
        for (auto const &j: jetReader->GetJets())
            cout << " pt: " << j.Pt() << ", eta: " << j.Eta() << ", b-tag: " <<
              j.BTag(BTagger::Algorithm::CSV) << ", flavour: " << j.GetParentID() << '\n';
        
        cout << "\nMET: " << jetReader->GetMET().Pt() << '\n';
        
        cout << "\nPile-up info:\n #PV: " << puReader->GetNumVertices() << ", rho: " <<
          puReader->GetRho() << '\n';
        
        cout << "\nGenerator-level particles:\n";
        
        for (auto const &p: genParticleReader->GetParticles())
        {
            cout << " PDG ID: " << p.GetPdgId() << ", daughters PDG ID:";
            
            for (auto const &d: p.GetDaughters())
                cout << " " << d->GetPdgId();
            
            cout << '\n';
        }
        
        cout << "\nNominal GEN-level weight: " << generatorReader->GetNominalWeight() << '\n';
        cout << "Weight for b-tagging scale factors: " << bTagReweighter->CalcWeight() << '\n';
        
        cout << "\n\n";
    }
    
    
    return EXIT_SUCCESS;
}

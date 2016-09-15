#include <mensura/core/BTagWPService.hpp>
#include <mensura/core/Dataset.hpp>
#include <mensura/core/Processor.hpp>

#include <mensura/extensions/BTagEffService.hpp>
#include <mensura/extensions/BTagSFService.hpp>
#include <mensura/extensions/BTagWeight.hpp>
#include <mensura/extensions/JetFilter.hpp>
#include <mensura/extensions/LeptonFilter.hpp>
#include <mensura/extensions/MetFilter.hpp>

#include <mensura/PECReader/PECGeneratorReader.hpp>
#include <mensura/PECReader/PECGenJetMETReader.hpp>
#include <mensura/PECReader/PECGenParticleReader.hpp>
#include <mensura/PECReader/PECInputData.hpp>
#include <mensura/PECReader/PECJetMETReader.hpp>
#include <mensura/PECReader/PECLeptonReader.hpp>
#include <mensura/PECReader/PECPileUpReader.hpp>
#include <mensura/PECReader/PECTriggerFilter.hpp>

#include <iostream>
#include <memory>


using namespace std;


int main()
{
    // Input dataset
    Dataset dataset(Dataset::Type::MC);
    dataset.AddFile("ttbar.root", 831.76, 1000000 /* a dummy value */);
    
    
    // Triggers
    list<TriggerRange> triggerRanges;
    triggerRanges.emplace_back(0, -1, "IsoMu24", 1e3 /* a dummy luminosity */, "IsoMu24");
    
    
    // Common definition of b-tagging that will be used everywhere
    BTagger const bTagger(BTagger::Algorithm::CMVA, BTagger::WorkingPoint::Medium);
    
    
    // Processor object
    Processor processor;
    
    
    // Register b-tagging services
    processor.RegisterService(new BTagWPService("BTagWP_80Xv1.json"));
    
    BTagEffService *bTagEffService = new BTagEffService("BTagEff_80Xv1.root");
    bTagEffService->SetDefaultEffLabel("ttbar");
    processor.RegisterService(bTagEffService);
    
    BTagSFService *bTagSFService = new BTagSFService(bTagger, "BTagSF_cMVAv2_80Xv1.csv");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Bottom, "ttbar");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Charm, "ttbar");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Light, "incl");
    processor.RegisterService(bTagSFService);
    
    
    // Register plugins
    processor.RegisterPlugin(new PECInputData);
    processor.RegisterPlugin(BuildPECTriggerFilter(false, triggerRanges));
    processor.RegisterPlugin(new PECLeptonReader);
    processor.RegisterPlugin(new LeptonFilter("LeptonFilter", Lepton::Flavour::Muon, 20., 2.4));
    processor.RegisterPlugin(new PECGenJetMETReader);
    
    PECJetMETReader *jetReader = new PECJetMETReader;
    jetReader->SetSelection(30., 2.4);
    jetReader->SetGenJetReader();
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
        {
            cout << " pt: " << j.Pt() << ", eta: " << j.Eta() << ", b-tag: " <<
              j.BTag(BTagger::Algorithm::CMVA) << ", flavour: " << j.GetParentID() << '\n';
            
            cout << "  pt of matched GEN jet: ";
            GenJet const *genJet = j.MatchedGenJet();
            
            if (genJet)
                cout << genJet->Pt();
            else
                cout << "(none)";
            
            cout << '\n';
        }
        
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
        cout << "Weight for b-tagging scale factors: " << bTagReweighter->GetWeight() << '\n';
        
        cout << "\n\n";
    }
    
    
    return EXIT_SUCCESS;
}

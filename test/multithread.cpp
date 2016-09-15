#include <mensura/core/BTagWPService.hpp>
#include <mensura/core/Dataset.hpp>
#include <mensura/core/RunManager.hpp>

#include <mensura/extensions/BasicKinematicsPlugin.hpp>
#include <mensura/extensions/BTagEffService.hpp>
#include <mensura/extensions/BTagSFService.hpp>
#include <mensura/extensions/BTagWeight.hpp>
#include <mensura/extensions/JetFilter.hpp>
#include <mensura/extensions/LeptonFilter.hpp>
#include <mensura/extensions/MetFilter.hpp>
#include <mensura/extensions/PileUpWeight.hpp>
#include <mensura/extensions/TFileService.hpp>
#include <mensura/extensions/WeightCollector.hpp>

#include <mensura/PECReader/PECGeneratorReader.hpp>
#include <mensura/PECReader/PECInputData.hpp>
#include <mensura/PECReader/PECJetMETReader.hpp>
#include <mensura/PECReader/PECLeptonReader.hpp>
#include <mensura/PECReader/PECPileUpReader.hpp>
#include <mensura/PECReader/PECTriggerFilter.hpp>

#include <iostream>
#include <list>


using namespace std;


int main()
{
    // Input datasets
    list<Dataset> datasets;
    string const filePrefix("/gridgroup/cms/popov/PECData/2015Charlie/");
    
    datasets.emplace_back(Dataset(Dataset::Type::MC));
    Dataset *d = &datasets.back();
    d->AddFile(filePrefix + "ttbar-pw_3.1.0_wdo_p1.root", 831.76, 97994442);
    d->AddFile(filePrefix + "ttbar-pw_3.1.0_wdo_p2.root", 831.76, 97994442);
    d->AddFile(filePrefix + "ttbar-pw_3.1.0_wdo_p3.root", 831.76, 97994442);
    d->AddFile(filePrefix + "ttbar-pw_3.1.0_wdo_p4.root", 831.76, 97994442);
    d->AddFile(filePrefix + "ttbar-pw_3.1.0_wdo_p5.root", 831.76, 97994442);
    //^ Only a fraction of available files included here
    
    
    // Triggers
    list<TriggerRange> triggerRanges;
    triggerRanges.emplace_back(TriggerRange(0, -1, {"IsoMu24", "IsoTkMu24"},
      1e3 /* a dummy luminosity */, {"IsoMu24", "IsoTkMu24"}));
    
    
    // Common definition of b-tagging that will be used everywhere
    BTagger const bTagger(BTagger::Algorithm::CMVA, BTagger::WorkingPoint::Tight);
    
    
    // Construct the run manager
    RunManager manager(datasets.begin(), datasets.end());
    
    
    // Register services
    manager.RegisterService(new TFileService("output/%"));
    
    manager.RegisterService(new BTagWPService("BTagWP_80Xv1.json"));
    
    BTagEffService *bTagEffService = new BTagEffService("BTagEff_80Xv1.root");
    bTagEffService->SetDefaultEffLabel("ttbar");
    manager.RegisterService(bTagEffService);
    
    BTagSFService *bTagSFService = new BTagSFService(bTagger, "BTagSF_cMVAv2_80Xv1.csv");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Bottom, "ttbar");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Charm, "ttbar");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Light, "incl");
    manager.RegisterService(bTagSFService);
    
    
    // Register plugins
    manager.RegisterPlugin(new PECInputData);
    manager.RegisterPlugin(BuildPECTriggerFilter(false, triggerRanges));
    manager.RegisterPlugin(new PECLeptonReader);
    manager.RegisterPlugin(new LeptonFilter("LeptonFilter", Lepton::Flavour::Muon, 22., 2.4));
    
    PECJetMETReader *jetReader = new PECJetMETReader;
    jetReader->SetSelection(30., 2.4);
    manager.RegisterPlugin(jetReader);
    
    JetFilter *jetFilter = new JetFilter(0., bTagger);
    jetFilter->AddSelectionBin(4, -1, 2, 2);
    manager.RegisterPlugin(jetFilter);
    
    manager.RegisterPlugin(new MetFilter(MetFilter::Mode::MtW, 40.));
    manager.RegisterPlugin(new PECPileUpReader);
    manager.RegisterPlugin(new PECGeneratorReader);
    
    // Plugins to reweight events
    manager.RegisterPlugin(new PileUpWeight("Run2016BCD_SingleMuon_v1_finebin.root",
      "simPUProfiles_80X.root", 0.05));
    manager.RegisterPlugin(new BTagWeight(bTagger));
    manager.RegisterPlugin(new WeightCollector({"PileUpWeight", "BTagWeight"}));
    
    
    // Finally, the plugin to calculate some observables
    manager.RegisterPlugin(new BasicKinematicsPlugin);
    
    
    // Process the datasets
    manager.Process(4);
    
    
    // Print numbers of processed and accepted events
    std::cout << '\n';
    manager.PrintSummary();
    
    
    return EXIT_SUCCESS;
}
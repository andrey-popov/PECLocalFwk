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
#include <mensura/extensions/TFileService.hpp>

#include <mensura/PECReader/PECGeneratorReader.hpp>
#include <mensura/PECReader/PECInputData.hpp>
#include <mensura/PECReader/PECJetMETReader.hpp>
#include <mensura/PECReader/PECLeptonReader.hpp>
#include <mensura/PECReader/PECPileUpReader.hpp>
#include <mensura/PECReader/PECTriggerFilter.hpp>

#include <list>


using namespace std;


int main()
{
    // Input datasets
    list<Dataset> datasets;
    string const filePrefix("/gridgroup/cms/popov/PECData/2015Charlie/");
    
    datasets.emplace_back(Dataset({Dataset::Process::ttbar}, Dataset::Generator::POWHEG));
    Dataset *d = &datasets.back();
    d->AddFile(filePrefix + "ttbar-pw_3.1.0_wdo_p1.root", 831.76, 97994442);
    d->AddFile(filePrefix + "ttbar-pw_3.1.0_wdo_p2.root", 831.76, 97994442);
    d->AddFile(filePrefix + "ttbar-pw_3.1.0_wdo_p3.root", 831.76, 97994442);
    d->AddFile(filePrefix + "ttbar-pw_3.1.0_wdo_p4.root", 831.76, 97994442);
    d->AddFile(filePrefix + "ttbar-pw_3.1.0_wdo_p5.root", 831.76, 97994442);
    //^ Only a fraction of available files included here
    
    
    // Triggers
    list<TriggerRange> triggerRanges;
    triggerRanges.emplace_back(TriggerRange(0, -1, {"IsoMu20", "IsoTkMu20"}, 2289.901,
      {"IsoMu20", "IsoTkMu20"}));
    
    
    // Common definition of b-tagging that will be used everywhere
    BTagger const bTagger(BTagger::Algorithm::CSV, BTagger::WorkingPoint::Tight);
    
    
    // Construct the run manager
    RunManager manager(datasets.begin(), datasets.end());
    
    
    // Register services
    manager.RegisterService(new TFileService("output"));
    
    manager.RegisterService(new BTagWPService);
    
    BTagEffService *bTagEffService = new BTagEffService("BTagEff_74X_v1.0.root");
    // bTagEffService->SetProcessLabel(Dataset::Process::ttSemilep, "ttbar-semilep");
    bTagEffService->SetDefaultProcessLabel("ttbar");
    manager.RegisterService(bTagEffService);
    
    BTagSFService *bTagSFService = new BTagSFService(bTagger, "BTagSF_74X_CSVv2.csv");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Bottom, "mujets");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Charm, "mujets");
    bTagSFService->SetMeasurement(BTagSFService::Flavour::Light, "comb");
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
    manager.RegisterPlugin(new BTagWeight(bTagger));
    
    
    // Finally, the plugin to calculate some observables
    manager.RegisterPlugin(new BasicKinematicsPlugin("basicTuples"));
    
    
    // Process the datasets
    manager.Process(4);
    
    
    return EXIT_SUCCESS;
}
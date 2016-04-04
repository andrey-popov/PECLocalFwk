#include <PECFwk/core/BTagWPService.hpp>
#include <PECFwk/core/Dataset.hpp>
#include <PECFwk/core/RunManager.hpp>

#include <PECFwk/extensions/BasicKinematicsPlugin.hpp>
#include <PECFwk/extensions/BTagEffService.hpp>
#include <PECFwk/extensions/BTagSFService.hpp>
#include <PECFwk/extensions/BTagWeight.hpp>
#include <PECFwk/extensions/JetFilter.hpp>
#include <PECFwk/extensions/LeptonFilter.hpp>
#include <PECFwk/extensions/MetFilter.hpp>
#include <PECFwk/extensions/TFileService.hpp>

#include <PECFwk/PECReader/PECGeneratorReader.hpp>
#include <PECFwk/PECReader/PECInputData.hpp>
#include <PECFwk/PECReader/PECJetMETReader.hpp>
#include <PECFwk/PECReader/PECLeptonReader.hpp>
#include <PECFwk/PECReader/PECPileUpReader.hpp>
#include <PECFwk/PECReader/PECTriggerFilter.hpp>

#include <list>


using namespace std;


int main()
{
    // Input datasets
    list<Dataset> datasets;
    string const filePrefix("/gridgroup/cms/popov/PECData/2015Bravo/");
    
    datasets.emplace_back(Dataset({Dataset::Process::ttbar}, Dataset::Generator::POWHEG));
    Dataset *d = &datasets.back();
    d->AddFile(filePrefix + "ttbar-pw_3.0.0_VmF_p1.root", 831.76, 96834559);
    d->AddFile(filePrefix + "ttbar-pw_3.0.0_VmF_p2.root", 831.76, 96834559);
    d->AddFile(filePrefix + "ttbar-pw_3.0.0_VmF_p3.root", 831.76, 96834559);
    d->AddFile(filePrefix + "ttbar-pw_3.0.0_VmF_p4.root", 831.76, 96834559);
    d->AddFile(filePrefix + "ttbar-pw_3.0.0_VmF_p5.root", 831.76, 96834559);
    //^ Only a fraction of available files included here
    
    
    // Triggers
    list<TriggerRange> triggerRanges;
    triggerRanges.emplace_back(0, -1, "IsoMu20", 2244.966, "IsoMu20");
    
    
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
    manager.RegisterPlugin(new LeptonFilter("LeptonFilter", Lepton::Flavour::Muon, 22., 2.1));
    
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
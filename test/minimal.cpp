#include <PECFwk/core/Dataset.hpp>
#include <PECFwk/core/Processor.hpp>

#include <PECFwk/core/BTagger.hpp>
#include <PECFwk/extensions/GenericEventSelection.hpp>
#include <PECFwk/extensions/BTagEfficiencies.hpp>
#include <PECFwk/extensions/BTagScaleFactors.hpp>
#include <PECFwk/extensions/WeightBTag.hpp>

#include <PECFwk/extensions/JetFilter.hpp>
#include <PECFwk/extensions/LeptonFilter.hpp>
#include <PECFwk/extensions/MetFilter.hpp>
#include <PECFwk/PECReader/PECGeneratorReader.hpp>
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
    #if 0
    // Define the b-tagging objects
    shared_ptr<BTagger const> bTagger(
     new BTagger(BTagger::Algorithm::CSV, BTagger::WorkingPoint::Tight));
    
    
    // Define the event selection
    GenericEventSelection sel(30., bTagger);
    sel.AddLeptonThreshold(Lepton::Flavour::Muon, 26.);
    sel.AddJetTagBin(2, 1);
    sel.AddJetTagBin(3, 1);
    sel.AddJetTagBin(3, 2);
    
    
    // Define datasets
    string const filePrefix("/gridgroup/cms/popov/PECData/2012Bravo/");
    double const brWlnu = 3 * 0.1080;
    list<Dataset> datasets;
    
    // ttbar
    datasets.emplace_back(Dataset({Dataset::Process::ttbar, Dataset::Process::ttSemilep},
     Dataset::Generator::MadGraph, Dataset::ShowerGenerator::Pythia));
    datasets.back().AddFile(filePrefix + "ttbar-semilep-mg-p1_53X.02.05_tTP_p1.root",
     234. * brWlnu * (1. - brWlnu) * 2, 24953451);
    datasets.back().AddFile(filePrefix + "ttbar-semilep-mg-p1_53X.02.05_tTP_p2.root",
     234. * brWlnu * (1. - brWlnu) * 2, 24953451);
    datasets.back().AddFile(filePrefix + "ttbar-semilep-mg-p1_53X.02.05_tTP_p3.root",
     234. * brWlnu * (1. - brWlnu) * 2, 24953451);
    datasets.back().AddFile(filePrefix + "ttbar-semilep-mg-p1_53X.02.05_tTP_p4.root",
     234. * brWlnu * (1. - brWlnu) * 2, 24953451);
    
     
    // Define the triggers
    list<TriggerRange> triggerRanges;
    triggerRanges.emplace_back(0, -1, "IsoMu24_eta2p1", 19.7e3, "IsoMu24_eta2p1");
    
    TriggerSelection triggerSel(triggerRanges);
    
    
    // Define reweighting for b-tagging
    BTagEfficiencies bTagEff("BTagEff_2012Bravo_v1.0.root", "in4_jPt30/");
    
    // Set a mapping from process codes to names of histograms with b-tagging efficiencies
    bTagEff.SetProcessLabel(Dataset::Process::ttSemilep, "ttbar-semilep");
    bTagEff.SetProcessLabel(Dataset::Process::ttchan, "t-tchan");
    bTagEff.SetProcessLabel(Dataset::Process::ttH, "ttH");
    bTagEff.SetProcessLabel(Dataset::Process::tHq, "tHq-nc");
    bTagEff.SetDefaultProcessLabel("ttbar-inc");
    
    BTagScaleFactors bTagSF(bTagger->GetAlgorithm());
    WeightBTag bTagReweighter(bTagger, bTagEff, bTagSF);
    
    
    // Perform dataset-specific initialization
    bTagReweighter.LoadPayload(datasets.front());
    
    
    // Build an instance of PECReader
    PECReader reader(datasets.front());
    reader.SetTriggerSelection(&triggerSel);
    reader.SetEventSelection(&sel);
    reader.SetBTagReweighter(&bTagReweighter);
    //reader.SetReadHardInteraction();
    reader.NextSourceFile();
    #endif
    
    
    // Datasets
    Dataset dataTTbar({Dataset::Process::ttbar, Dataset::Process::ttSemilep},
     Dataset::Generator::MadGraph, Dataset::ShowerGenerator::Pythia);
    dataTTbar.AddFile("/gridgroup/cms/popov/Analyses/ZPrimeToTT/2016.01.15_Grid-campaign/PEC/"
     "ExampleOutputFiles/ttbar-pw_3.0.0_VmF_1.root", 1., 1);
    
    
    // Triggers
    list<TriggerRange> triggerRanges;
    triggerRanges.emplace_back(0, -1, "IsoMu20", 2.2e3, "IsoMu20");
    
    
    // Processor object and plugins
    Processor processor;
    
    processor.RegisterPlugin(new PECInputData);
    processor.RegisterPlugin(BuildPECTriggerFilter(false, triggerRanges));
    processor.RegisterPlugin(new PECLeptonReader);
    processor.RegisterPlugin(new LeptonFilter("LeptonFilter", Lepton::Flavour::Muon, 22., 2.1));
    
    PECJetMETReader *jetReader = new PECJetMETReader;
    jetReader->SetSelection(30., 2.4);
    processor.RegisterPlugin(jetReader);
    
    JetFilter *jetFilter = new JetFilter;
    jetFilter->AddSelectionBin(4, -1, 2, 2);
    processor.RegisterPlugin(jetFilter);
    
    processor.RegisterPlugin(new MetFilter(MetFilter::Mode::MtW, 40.));
    processor.RegisterPlugin(new PECPileUpReader);
    processor.RegisterPlugin(new PECGeneratorReader);
    
    processor.OpenDataset(dataTTbar);
    
    
    PECInputData const *inputData =
      dynamic_cast<PECInputData const *>(processor.GetPlugin("InputData"));
    LeptonReader const *leptonReader =
      dynamic_cast<LeptonReader const *>(processor.GetPlugin("Leptons"));
    PileUpReader const *puReader =
      dynamic_cast<PileUpReader const *>(processor.GetPlugin("PileUp"));
    PECGeneratorReader const *generatorReader =
      dynamic_cast<PECGeneratorReader const *>(processor.GetPlugin("Generator"));
    
    
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
            cout << " flavour: " << int(l.GetFlavour()) << ", pt: " << l.Pt() << ", iso: " <<
              l.RelIso() << ", dB: " << l.DB() << '\n';
        
        cout << "\nLoose leptons:\n";
        
        for (auto const &l: leptonReader->GetLooseLeptons())
            cout << " flavour: " << int(l.GetFlavour()) << ", pt: " << l.Pt() << ", iso: " <<
              l.RelIso() << ", dB: " << l.DB() << '\n';
        
        cout << "\nAnalysis jets:\n";
        
        for (auto const &j: jetReader->GetJets())
            cout << " pt: " << j.Pt() << ", eta: " << j.Eta() << ", b-tag: " << j.CSV() <<
              ", flavour: " << j.GetParentID() << '\n';
        
        cout << "\nMET: " << jetReader->GetMET().Pt() << '\n';
        
        cout << "\nPile-up info:\n #PV: " << puReader->GetNumVertices() << ", rho: " <<
          puReader->GetRho() << '\n';
        
        cout << "\nNominal GEN-level weight: " << generatorReader->GetNominalWeight() << '\n';
        
        cout << "\n\n";
    }
    
    
    return 0;
}

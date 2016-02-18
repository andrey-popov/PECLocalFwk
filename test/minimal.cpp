#include <PECFwk/core/Dataset.hpp>
#include <PECFwk/core/Processor.hpp>

#include <PECFwk/core/PECReader.hpp>
#include <PECFwk/core/BTagger.hpp>
#include <PECFwk/extensions/GenericEventSelection.hpp>
#include <PECFwk/extensions/BTagEfficiencies.hpp>
#include <PECFwk/extensions/BTagScaleFactors.hpp>
#include <PECFwk/extensions/WeightBTag.hpp>
#include <PECFwk/extensions/TriggerSelection.hpp>

#include <PECFwk/PECReader/PECInputData.hpp>
#include <PECFwk/PECReader/PECLeptonReader.hpp>

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
    
    
    Dataset dataTTbar({Dataset::Process::ttbar, Dataset::Process::ttSemilep},
     Dataset::Generator::MadGraph, Dataset::ShowerGenerator::Pythia);
    dataTTbar.AddFile("/gridgroup/cms/popov/Analyses/ZPrimeToTT/2016.01.15_Grid-campaign/PEC/"
     "ExampleOutputFiles/ttbar-pw_3.0.0_VmF_1.root", 1., 1);
    
    Processor processor;
    
    processor.RegisterPlugin(new PECInputData);
    processor.RegisterPlugin(new PECLeptonReader);
    
    processor.OpenDataset(dataTTbar);
    
    LeptonReader const *leptonReader =
      dynamic_cast<LeptonReader const *>(processor.GetPlugin("Leptons"));
    
    
    // Loop over few events
    for (unsigned i = 0; i < 20; ++i)
    {
        cout << "Event " << i << '\n';
        processor.ProcessEvent();
        
        cout << "Tight leptons:\n";
        
        for (auto const &l: leptonReader->GetLeptons())
            cout << " flavour: " << int(l.GetFlavour()) << ", pt: " << l.Pt() << ", iso: " <<
              l.RelIso() << ", dB: " << l.DB() << "\n";
        
        cout << "\nLoose leptons:\n";
        
        for (auto const &l: leptonReader->GetLooseLeptons())
            cout << " flavour: " << int(l.GetFlavour()) << ", pt: " << l.Pt() << ", iso: " <<
              l.RelIso() << ", dB: " << l.DB() << "\n";
        
        
        #if 0
        reader.NextEvent();
        
        cout << "Tight leptons' pts:";
        
        for (auto const &l: reader.GetLeptons())
            cout << " pt: " << l.Pt() << ", iso: " << l.RelIso() << ", dB: " << l.DB();
        
        cout << "\nAnalysis jets' pts:";
        
        for (auto const &j: reader.GetJets())
            cout << " " << j.Pt();
        
        cout << "\nAdditional jets' pts:";
        
        for (auto const &j: reader.GetAdditionalJets())
            cout << " " << j.Pt();
        
        cout << "\nNeutrino's pt: " << reader.GetNeutrino().Pt() << ", pz: " <<
         reader.GetNeutrino().P4().Pz();
        
        /*
        cout << "\nHard generator-level particles:";
        
        for (auto const &p: reader.GetHardGenParticles())
        {
            cout << "\n PDG ID: " << p.GetPdgId() <<
             "\n # mothers: " << p.GetMothers().size() <<
             "\n # daughters: " << p.GetDaughters().size();
        }
        */
        
        cout << "\nEvent weight: " << reader.GetCentralWeight();
        #endif
        
        cout << "\n\n";
    }
    
    
    return 0;
}

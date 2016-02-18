#include <PECFwk/PECReader/PECJetMETReader.hpp>

#include <PECFwk/core/Processor.hpp>
#include <PECFwk/PECReader/PECInputData.hpp>


PECJetMETReader::PECJetMETReader(std::string name /*= "JetMET"*/):
    JetMETReader(name),
    inputDataPluginName("InputData"),
    inputDataPlugin(nullptr),
    treeName("pecJetMET/JetMET"),
    bfJetPointer(&bfJets), bfMETPointer(&bfMETs)
{}


PECJetMETReader::~PECJetMETReader()
{}


void PECJetMETReader::BeginRun(Dataset const &)
{
    // Save pointer to the plugin providing access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(
      GetMaster().GetPluginBefore(inputDataPluginName, GetName()));
    
    
    // Set up the tree
    inputDataPlugin->LoadTree(treeName);
    TTree *tree = inputDataPlugin->ExposeTree(treeName);
    tree->SetBranchAddress("jets", &bfJetPointer);
    tree->SetBranchAddress("METs", &bfMETPointer);
}


Plugin *PECJetMETReader::Clone() const
{
    return new PECJetMETReader(GetName());
}


bool PECJetMETReader::ProcessEvent()
{
    // Clear vector with jets from the previous event
    jets.clear();
    
    
    // Read jets and MET
    inputDataPlugin->ReadEventFromTree(treeName);
    
    
    // Process jets in the current event
    for (pec::Jet const &j: bfJets)
    {
        TLorentzVector p4;
        p4.SetPtEtaPhiM(j.Pt(), j.Eta(), j.Phi(), j.M());
        
        
        // Loose selection
        if (not j.TestBit(0) /* "loose" jet ID */)
            continue;
        
        
        // Build the jet object
        Jet jet(p4);  // Here the momentum is assumed to be fully corrected
        
        jet.SetCSV(j.BTagCSV());
        jet.SetArea(j.Area());
        jet.SetCharge(j.Charge());
        jet.SetPullAngle(j.PullAngle());
        
        jet.SetParentID(j.Flavour());
        
        
        jets.push_back(jet);
    }
    
    
    // Make sure collection of jets is ordered in transverse momentum
    std::sort(jets.rbegin(), jets.rend());
    
    
    // Copy corrected MET
    pec::Candidate const &correctedMET = bfMETs.at(0);
    met.SetPtEtaPhiM(correctedMET.Pt(), 0., correctedMET.Phi(), 0.);
    
    
    
    // Since this reader does not have access to the input file, it does not know when there are
    //no more events in the dataset and thus always returns true
    return true;
}


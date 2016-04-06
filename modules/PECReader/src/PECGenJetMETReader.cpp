#include <PECFwk/PECReader/PECGenJetMETReader.hpp>

#include <PECFwk/core/Processor.hpp>
#include <PECFwk/core/ROOTLock.hpp>
#include <PECFwk/PECReader/PECInputData.hpp>

#include <TVector2.h>

#include <cmath>
#include <limits>


PECGenJetMETReader::PECGenJetMETReader(std::string name /*= "GenJetMET"*/):
    GenJetMETReader(name),
    inputDataPluginName("InputData"),
    inputDataPlugin(nullptr),
    treeName("pecGenJetMET/GenJetMET"),
    bfJetPointer(&bfJets), bfMETPointer(&bfMETs),
    minPt(0.), maxAbsEta(std::numeric_limits<double>::infinity())
{}


PECGenJetMETReader::PECGenJetMETReader(PECGenJetMETReader const &src) noexcept:
    GenJetMETReader(src),
    inputDataPluginName(src.inputDataPluginName),
    inputDataPlugin(src.inputDataPlugin),
    treeName(src.treeName),
    bfJetPointer(&bfJets), bfMETPointer(&bfMETs),
    minPt(src.minPt), maxAbsEta(src.maxAbsEta)
{}


PECGenJetMETReader::~PECGenJetMETReader() noexcept
{}


void PECGenJetMETReader::BeginRun(Dataset const &)
{
    // Save pointer to the plugin providing access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(GetDependencyPlugin(inputDataPluginName));
    
    
    // Set up the tree
    inputDataPlugin->LoadTree(treeName);
    TTree *tree = inputDataPlugin->ExposeTree(treeName);
    ROOTLock::Lock();
    tree->SetBranchAddress("jets", &bfJetPointer);
    tree->SetBranchAddress("METs", &bfMETPointer);
    ROOTLock::Unlock();
}


Plugin *PECGenJetMETReader::Clone() const
{
    return new PECGenJetMETReader(*this);
}


double PECGenJetMETReader::GetJetRadius() const
{
    return 0.4;
}


void PECGenJetMETReader::SetSelection(double minPt_, double maxAbsEta_)
{
    minPt = minPt_;
    maxAbsEta = maxAbsEta_;
}


bool PECGenJetMETReader::ProcessEvent()
{
    // Clear vector with jets from the previous event
    jets.clear();
    
    
    // Read jets and MET
    inputDataPlugin->ReadEventFromTree(treeName);
    
    
    // Process jets in the current event
    for (pec::GenJet const &j: bfJets)
    {
        TLorentzVector p4;
        p4.SetPtEtaPhiM(j.Pt(), j.Eta(), j.Phi(), j.M());
        
        
        // User-defined selection on momentum
        if (p4.Pt() < minPt or fabs(p4.Eta()) > maxAbsEta)
            continue;
        
        
        // Build the jet object
        GenJet jet(p4);
        jet.SetMultiplicities(j.BottomMult(), j.CharmMult());
        
        
        jets.push_back(jet);
    }
    
    
    // Make sure collection of jets is ordered in transverse momentum
    std::sort(jets.rbegin(), jets.rend());
    
    
    // Copy MET. There is only one element in the collection
    pec::Candidate const &srcMET = bfMETs.at(0);
    met.SetPtEtaPhiM(srcMET.Pt(), 0., srcMET.Phi(), 0.);
    
    
    
    // Since this reader does not have access to the input file, it does not know when there are
    //no more events in the dataset and thus always returns true
    return true;
}


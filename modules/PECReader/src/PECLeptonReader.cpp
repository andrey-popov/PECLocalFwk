#include <mensura/PECReader/PECLeptonReader.hpp>

#include <mensura/core/Processor.hpp>
#include <mensura/core/ROOTLock.hpp>
#include <mensura/PECReader/PECInputData.hpp>

#include <algorithm>
#include <cmath>


PECLeptonReader::PECLeptonReader(std::string const name /*= "Leptons"*/):
    LeptonReader(name),
    inputDataPluginName("InputData"), inputDataPlugin(nullptr),
    electronTreeName("pecElectrons/Electrons"), bfElectronPointer(&bfElectrons),
    muonTreeName("pecMuons/Muons"), bfMuonPointer(&bfMuons)
{}


PECLeptonReader::PECLeptonReader(PECLeptonReader const &src) noexcept:
    LeptonReader(src),
    inputDataPluginName(src.inputDataPluginName),
    inputDataPlugin(src.inputDataPlugin),
    electronTreeName(src.electronTreeName),
    bfElectronPointer(&bfElectrons),
    muonTreeName(src.muonTreeName),
    bfMuonPointer(&bfMuons)
{}


PECLeptonReader::~PECLeptonReader() noexcept
{}


void PECLeptonReader::BeginRun(Dataset const &)
{
    // Save pointer to the plugin providing access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(GetDependencyPlugin(inputDataPluginName));
    
    
    // Set up the trees. Branches with properties that are currently not utilized, are disabled
    inputDataPlugin->LoadTree(electronTreeName);
    inputDataPlugin->LoadTree(muonTreeName);
    
    ROOTLock::Lock();
    TTree *t = inputDataPlugin->ExposeTree(electronTreeName);
    t->SetBranchStatus("electrons.dB", false);
    t->SetBranchStatus("electrons.mvaId*", false);
    t->SetBranchAddress("electrons", &bfElectronPointer);
    
    t = inputDataPlugin->ExposeTree(muonTreeName);
    t->SetBranchStatus("muons.dB", false);
    t->SetBranchAddress("muons", &bfMuonPointer);
    ROOTLock::Unlock();
}


Plugin *PECLeptonReader::Clone() const
{
    return new PECLeptonReader(*this);
}


bool PECLeptonReader::ProcessEvent()
{
    // Clear vectors with leptons from the previous event
    leptons.clear();
    looseLeptons.clear();
    
    
    // Read and process electrons in the current event
    inputDataPlugin->ReadEventFromTree(electronTreeName);
    
    for (pec::Electron const &l: bfElectrons)
    {
        TLorentzVector p4;
        p4.SetPtEtaPhiM(l.Pt(), l.Eta(), l.Phi(), 0.511e-3);
        
        
        // Selection to define a loose electron
        if (p4.Pt() < 20. or fabs(p4.Eta()) > 2.5 or not l.BooleanID(0) /* "veto" cut-based ID */)
        //^ The cut-based ID already includes selection on isolation
            continue;
        
        Lepton lepton(Lepton::Flavour::Electron, p4);
        lepton.SetRelIso(l.RelIso());
        lepton.SetCharge(l.Charge());
        lepton.SetUserFloat("etaSC", l.EtaSC());
        
        looseLeptons.push_back(lepton);
        
        
        // Further selection for a tight electron
        double const absEtaSC = std::abs(l.EtaSC());
        
        if (not l.BooleanID(3) /* "tight" cut-based ID */ or
          (absEtaSC > 1.4442 and absEtaSC < 1.5660) /* EB-EE gap */)
            continue;
        
        leptons.push_back(lepton);
    }
    
    
    // Read and process muons in the current event
    inputDataPlugin->ReadEventFromTree(muonTreeName);
    
    for (pec::Muon const &l: bfMuons)
    {
        TLorentzVector p4;
        p4.SetPtEtaPhiM(l.Pt(), l.Eta(), l.Phi(), 0.105);
        
        
        // Selection to define a loose muon
        if (p4.Pt() < 10. or fabs(p4.Eta()) > 2.4 or l.RelIso() > 0.25 or
          not l.TestBit(0) /* "loose" ID */)
            continue;
        
        Lepton lepton(Lepton::Flavour::Muon, p4);
        lepton.SetRelIso(l.RelIso());
        lepton.SetCharge(l.Charge());
        
        looseLeptons.push_back(lepton);
        
        
        // Further selection for a tight muon
        if (l.RelIso() > 0.15 or not l.TestBit(2) /* "tight" ID */)
            continue;
        
        leptons.push_back(lepton);
    }
    
    
    // Make sure both collections are ordered in transverse momentum
    std::sort(leptons.rbegin(), leptons.rend());
    std::sort(looseLeptons.rbegin(), looseLeptons.rend());
    
    
    // Since this reader does not have access to the input file, it does not know when there are
    //no more events in the dataset and thus always returns true
    return true;
}

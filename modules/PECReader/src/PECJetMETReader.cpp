#include <PECFwk/PECReader/PECJetMETReader.hpp>

#include <PECFwk/core/Processor.hpp>
#include <PECFwk/PECReader/PECInputData.hpp>

#include <TVector2.h>

#include <cmath>
#include <limits>


PECJetMETReader::PECJetMETReader(std::string name /*= "JetMET"*/):
    JetMETReader(name),
    inputDataPluginName("InputData"),
    inputDataPlugin(nullptr),
    treeName("pecJetMET/JetMET"),
    bfJetPointer(&bfJets), bfMETPointer(&bfMETs),
    minPt(0.), maxAbsEta(std::numeric_limits<double>::infinity()),
    leptonPluginName("Leptons"), leptonPlugin(nullptr), leptonDR2(0.3 * 0.3)
{}


PECJetMETReader::PECJetMETReader(PECJetMETReader const &src) noexcept:
    JetMETReader(src),
    inputDataPluginName(src.inputDataPluginName),
    inputDataPlugin(src.inputDataPlugin),
    treeName(src.treeName),
    bfJetPointer(&bfJets), bfMETPointer(&bfMETs),
    minPt(src.minPt), maxAbsEta(src.maxAbsEta),
    leptonPluginName(src.leptonPluginName), leptonPlugin(src.leptonPlugin),
    leptonDR2(src.leptonDR2)
{}


PECJetMETReader::~PECJetMETReader()
{}


void PECJetMETReader::ConfigureLeptonCleaning(std::string const leptonPluginName_ /*= "Leptons"*/,
  double dR /*= 0.3*/)
{
    leptonPluginName = leptonPluginName_;
    leptonDR2 = dR * dR;
}


void PECJetMETReader::BeginRun(Dataset const &)
{
    // Save pointer to the plugin providing access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(
      GetMaster().GetPluginBefore(inputDataPluginName, GetName()));
    
    
    // Save pointer to plugin that produces leptons
    if (leptonPluginName != "")
        leptonPlugin = dynamic_cast<LeptonReader const *>(
          GetMaster().GetPluginBefore(leptonPluginName, GetName()));
    
    
    // Set up the tree
    inputDataPlugin->LoadTree(treeName);
    TTree *tree = inputDataPlugin->ExposeTree(treeName);
    tree->SetBranchAddress("jets", &bfJetPointer);
    tree->SetBranchAddress("METs", &bfMETPointer);
}


Plugin *PECJetMETReader::Clone() const
{
    return new PECJetMETReader(*this);
}


void PECJetMETReader::SetSelection(double minPt_, double maxAbsEta_)
{
    minPt = minPt_;
    maxAbsEta = maxAbsEta_;
}


bool PECJetMETReader::ProcessEvent()
{
    // Clear vector with jets from the previous event
    jets.clear();
    
    
    // Read jets and MET
    inputDataPlugin->ReadEventFromTree(treeName);
    
    // Collection of leptons against which jets will be cleaned
    auto const *leptonsForCleaning = (leptonPlugin) ? &leptonPlugin->GetLeptons() : nullptr;
    
    
    // Process jets in the current event
    for (pec::Jet const &j: bfJets)
    {
        TLorentzVector p4;
        p4.SetPtEtaPhiM(j.Pt(), j.Eta(), j.Phi(), j.M());
        
        
        // Loose physics selection
        if (not j.TestBit(0) /* "loose" jet ID */)
            continue;
        
        // User-defined selection on momentum
        if (p4.Pt() < minPt or fabs(p4.Eta()) > maxAbsEta)
            continue;
        
        
        // Peform cleaning against leptons if enabled
        if (leptonsForCleaning)
        {
            bool overlap = false;
            
            for (auto const &l: *leptonsForCleaning)
            {
                double const dR2 = std::pow(p4.Eta() - l.Eta(), 2) +
                  std::pow(TVector2::Phi_mpi_pi(p4.Phi() - l.Phi()), 2);
                //^ Do not use TLorentzVector::DeltaR to avoid calculating sqrt
                
                if (dR2 < leptonDR2)
                {
                    overlap = true;
                    break;
                }
            }
            
            if (overlap)
                continue;
        }
        
        
        // Build the jet object
        Jet jet(p4);  // Here the momentum is assumed to be fully corrected
        
        jet.SetBTag(BTagger::Algorithm::CSV, j.BTagCSV());
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


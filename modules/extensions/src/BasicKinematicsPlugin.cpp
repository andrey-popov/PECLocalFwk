#include <PECFwk/extensions/BasicKinematicsPlugin.hpp>

#include <PECFwk/core/LeptonReader.hpp>
#include <PECFwk/core/JetMETReader.hpp>
#include <PECFwk/core/PileUpReader.hpp>
#include <PECFwk/core/Processor.hpp>
#include <PECFwk/core/PhysicsObjects.hpp>
#include <PECFwk/core/ROOTLock.hpp>

#include <sys/stat.h>


BasicKinematicsPlugin::BasicKinematicsPlugin(std::string const &name,
  std::string const &outDirectory_):
    AnalysisPlugin(name),
    leptonPluginName("Leptons"), leptonPlugin(nullptr),
    jetmetPluginName("JetMET"), jetmetPlugin(nullptr),
    puPluginName("PileUp"), puPlugin(nullptr),
    outDirectory(outDirectory_)
{
    CreateOutputDirectory();
}


BasicKinematicsPlugin::BasicKinematicsPlugin(std::string const &outDirectory_):
    AnalysisPlugin("BasicKinematics"),
    leptonPluginName("Leptons"), leptonPlugin(nullptr),
    jetmetPluginName("JetMET"), jetmetPlugin(nullptr),
    puPluginName("PileUp"), puPlugin(nullptr),
    outDirectory(outDirectory_)
{
    CreateOutputDirectory();
}


BasicKinematicsPlugin::~BasicKinematicsPlugin() noexcept
{}


BasicKinematicsPlugin::BasicKinematicsPlugin(BasicKinematicsPlugin const &src):
    AnalysisPlugin(src),
    leptonPluginName(src.leptonPluginName), leptonPlugin(nullptr),
    jetmetPluginName(src.jetmetPluginName), jetmetPlugin(nullptr),
    puPluginName(src.puPluginName), puPlugin(nullptr),
    outDirectory(src.outDirectory)
{}


void BasicKinematicsPlugin::BeginRun(Dataset const &dataset)
{
    // Save pointers to reader plugins
    leptonPlugin = dynamic_cast<LeptonReader const *>(
      GetMaster().GetPluginBefore(leptonPluginName, GetName()));
    
    jetmetPlugin = dynamic_cast<JetMETReader const *>(
      GetMaster().GetPluginBefore(jetmetPluginName, GetName()));
    
    puPlugin = dynamic_cast<PileUpReader const *>(
      GetMaster().GetPluginBefore(puPluginName, GetName()));
    
    
    // Creation of ROOT objects is not thread-safe and must be protected
    ROOTLock::Lock();
    
    // Create the output file
    file.reset(
      new TFile((outDirectory + dataset.GetFiles().front().GetBaseName() + ".root").c_str(),
        "recreate"));
    
    // Create the tree
    tree.reset(new TTree("Vars", "Basic kinematical variables"));
    
    // End of critical block
    ROOTLock::Unlock();
    
    
    // Assign branch addresses
    tree->Branch("Pt_Lep", &Pt_Lep);
    tree->Branch("Eta_Lep", &Eta_Lep);
    tree->Branch("Pt_J1", &Pt_J1);
    tree->Branch("Eta_J1", &Eta_J1);
    tree->Branch("Pt_J2", &Pt_J2);
    tree->Branch("Eta_J2", &Eta_J2);
    tree->Branch("M_J1J2", &M_J1J2);
    tree->Branch("DR_J1J2", &DR_J1J2);
    tree->Branch("MET", &MET);
    tree->Branch("MtW", &MtW);
    tree->Branch("nPV", &nPV);
}


Plugin *BasicKinematicsPlugin::Clone() const
{
    return new BasicKinematicsPlugin(*this);
}


void BasicKinematicsPlugin::EndRun()
{
    // Operations with ROOT objects performed here are not thread-safe and must be guarded
    ROOTLock::Lock();
    
    // Write the tree and close the file
    file->cd();
    tree->Write("", TObject::kOverwrite);
    
    // Delete the objects
    tree.reset();
    file.reset();
    
    ROOTLock::Unlock();
}


void BasicKinematicsPlugin::CreateOutputDirectory()
{
    // Make sure the directory path ends with a slash
    if (outDirectory.back() != '/')
        outDirectory += '/';
    
    // Create the output directory if it does not exist
    struct stat dirStat;
    
    if (stat(outDirectory.c_str(), &dirStat) != 0)  // the directory does not exist
        mkdir(outDirectory.c_str(), 0755);
}


bool BasicKinematicsPlugin::ProcessEvent()
{
    auto const &leptons = leptonPlugin->GetLeptons();
    auto const &met = jetmetPlugin->GetMET();
    
    if (leptons.size() > 0)
    {
        auto const &lep = leptons.front();
        
        Pt_Lep = lep.Pt();
        Eta_Lep = lep.Eta();
        MtW = sqrt(pow(lep.Pt() + met.Pt(), 2) - pow(lep.P4().Px() + met.P4().Px(), 2) -
         pow(lep.P4().Py() + met.P4().Py(), 2));
    }
    else
        Pt_Lep = Eta_Lep = MtW = 0.;
    
    
    auto const &jets = jetmetPlugin->GetJets();
    Pt_J1 = Eta_J1 = Pt_J2 = Eta_J2 = M_J1J2 = DR_J1J2 = 0.;
    
    if (jets.size() > 0)
    {
        Pt_J1 = jets[0].Pt();
        Eta_J1 = jets[0].Eta();
    }
    
    if (jets.size() > 1)
    {
        Pt_J2 = jets[1].Pt();
        Eta_J2 = jets[1].Eta();
        
        M_J1J2 = (jets[0].P4() + jets[1].P4()).M();
        DR_J1J2 = jets[0].P4().DeltaR(jets[1].P4());
    }
    
    
    MET = met.Pt();
    nPV = puPlugin->GetNumVertices();
    
    
    tree->Fill();
    return true;
}

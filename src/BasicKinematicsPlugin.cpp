#include <mensura/BasicKinematicsPlugin.hpp>

#include <mensura/LeptonReader.hpp>
#include <mensura/JetMETReader.hpp>
#include <mensura/PileUpReader.hpp>
#include <mensura/Processor.hpp>
#include <mensura/PhysicsObjects.hpp>
#include <mensura/ROOTLock.hpp>
#include <mensura/TFileService.hpp>
#include <mensura/WeightCollector.hpp>


BasicKinematicsPlugin::BasicKinematicsPlugin(std::string const name /*= "BasicKinematics"*/):
    AnalysisPlugin(name),
    fileServiceName("TFileService"), fileService(nullptr),
    leptonPluginName("Leptons"), leptonPlugin(nullptr),
    jetmetPluginName("JetMET"), jetmetPlugin(nullptr),
    puPluginName("PileUp"), puPlugin(nullptr),
    weightCollectorName("EventWeights"), weightCollector(nullptr)
{}


BasicKinematicsPlugin::~BasicKinematicsPlugin() noexcept
{}


BasicKinematicsPlugin::BasicKinematicsPlugin(BasicKinematicsPlugin const &src):
    AnalysisPlugin(src),
    fileServiceName(src.fileServiceName), fileService(nullptr),
    leptonPluginName(src.leptonPluginName), leptonPlugin(nullptr),
    jetmetPluginName(src.jetmetPluginName), jetmetPlugin(nullptr),
    puPluginName(src.puPluginName), puPlugin(nullptr),
    weightCollectorName(src.weightCollectorName), weightCollector(nullptr)
{}


void BasicKinematicsPlugin::BeginRun(Dataset const &dataset)
{
    // Save pointer to the file service
    fileService = dynamic_cast<TFileService const *>(GetMaster().GetService(fileServiceName));
    
    // Save pointers to reader plugins
    leptonPlugin = dynamic_cast<LeptonReader const *>(GetDependencyPlugin(leptonPluginName));
    jetmetPlugin = dynamic_cast<JetMETReader const *>(GetDependencyPlugin(jetmetPluginName));
    puPlugin = dynamic_cast<PileUpReader const *>(GetDependencyPlugin(puPluginName));
    
    // Save pointer to a plugin that aggregates event weights
    if (dataset.IsMC())
        weightCollector =
          dynamic_cast<WeightCollector const *>(GetDependencyPlugin(weightCollectorName));
    
    
    // Create the output tree in the root directory of the output file
    tree = fileService->Create<TTree>("", "Vars", "Basic kinematical variables");
    
    
    // Declare branches. This is not a thread-safe operation, and this it must be protected
    ROOTLock::Lock();
    
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
    
    if (dataset.IsMC())
        tree->Branch("weight", &weight);
    
    ROOTLock::Unlock();
}


Plugin *BasicKinematicsPlugin::Clone() const
{
    return new BasicKinematicsPlugin(*this);
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
    
    
    if (weightCollector)  // defined only in simulation
        weight = weightCollector->GetWeight();
    
    
    tree->Fill();
    return true;
}

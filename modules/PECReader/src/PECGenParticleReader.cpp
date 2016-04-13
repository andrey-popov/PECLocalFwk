#include <mensura/PECReader/PECGenParticleReader.hpp>

#include <mensura/core/Processor.hpp>
#include <mensura/core/ROOTLock.hpp>
#include <mensura/PECReader/PECInputData.hpp>

#include <algorithm>


PECGenParticleReader::PECGenParticleReader(std::string const name /*= "GenParticles"*/):
    GenParticleReader(name),
    inputDataPluginName("InputData"), inputDataPlugin(nullptr),
    treeName("pecGenParticles/HardInteraction"), bfParticlePointer(&bfParticles)
{}


PECGenParticleReader::PECGenParticleReader(PECGenParticleReader const &src) noexcept:
    GenParticleReader(src),
    inputDataPluginName(src.inputDataPluginName),
    inputDataPlugin(src.inputDataPlugin),
    treeName(src.treeName),
    bfParticlePointer(&bfParticles)
{}


PECGenParticleReader::~PECGenParticleReader() noexcept
{}


void PECGenParticleReader::BeginRun(Dataset const &)
{
    // Save pointer to the plugin providing access to input data
    inputDataPlugin = dynamic_cast<PECInputData const *>(GetDependencyPlugin(inputDataPluginName));
    
    
    // Set up the tree
    inputDataPlugin->LoadTree(treeName);
    
    ROOTLock::Lock();
    TTree *t = inputDataPlugin->ExposeTree(treeName);
    t->SetBranchAddress("particles", &bfParticlePointer);
    ROOTLock::Unlock();
}


Plugin *PECGenParticleReader::Clone() const
{
    return new PECGenParticleReader(*this);
}


bool PECGenParticleReader::ProcessEvent()
{
    // Clear the vector with particles from the previous event
    particles.clear();
    
    
    // Read the source tree
    inputDataPlugin->ReadEventFromTree(treeName);
    
    
    // Construct particles in the standard format of the framework
    particles.reserve(bfParticles.size());
    
    for (pec::GenParticle const &p: bfParticles)
    {
        TLorentzVector p4;
        p4.SetPtEtaPhiM(p.Pt(), p.Eta(), p.Phi(), p.M());
        particles.emplace_back(p4, p.PdgId());
    }
    
    
    // Set mother-daughter relations
    for (unsigned i = 0; i < bfParticles.size(); ++i)
    {
        int iMother1 = bfParticles.at(i).FirstMotherIndex();
        
        if (iMother1 >= 0 and unsigned(iMother1) < bfParticles.size())
        {
            particles.at(i).AddMother(&particles.at(iMother1));
            particles.at(iMother1).AddDaughter(&particles.at(i));
        }
        
        
        int iMother2 = bfParticles.at(i).LastMotherIndex();
        
        if (iMother2 >= 0 and unsigned(iMother2) < bfParticles.size() and iMother2 != iMother1)
        {
            particles.at(i).AddMother(&particles.at(iMother2));
            particles.at(iMother2).AddDaughter(&particles.at(i));
        }
    }
    
    
    // Since this reader does not have access to the input file, it does not know when there are
    //no more events in the dataset and thus always returns true
    return true;
}

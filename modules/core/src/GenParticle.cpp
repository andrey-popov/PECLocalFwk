#include <mensura/core/GenParticle.hpp>

#include <algorithm>


GenParticle::GenParticle() noexcept:
    Candidate(),
    pdgId(0)
{}


GenParticle::GenParticle(TLorentzVector const &p4_, int pdgId_ /*= 0*/) noexcept:
    Candidate(p4_),
    pdgId(pdgId_)
{}


void GenParticle::AddDaughter(GenParticle const *p)
{
    daughters.push_back(p);
}


void GenParticle::AddMother(GenParticle const *p)
{
    mothers.push_back(p);
}


GenParticle const *GenParticle::FindFirstDaughter(std::initializer_list<int> const &pdgIds) const
{
    for (auto const &daughterPointer: daughters)
    {
        if (std::any_of(pdgIds.begin(), pdgIds.end(),
          [=](int pdgId){return (pdgId == daughterPointer->pdgId);}))
        return daughterPointer;
    }
    
    return nullptr;
}


GenParticle const *GenParticle::FindFirstDaughterRecursive(
  std::initializer_list<int> const &pdgIds) const
{
    // First check PDG ID of this particle
    if (std::any_of(pdgIds.begin(), pdgIds.end(), [this](int id){return (this->pdgId == id);}))
        return this;
    
    // If this is not the particle that is being looked for, check all the daughters
    for (auto const &daughterPointer: daughters)
    {
        auto const p = daughterPointer->FindFirstDaughterRecursive(pdgIds);
        
        if (p != nullptr)
            return p;
    }
    
    // If the control reaches this point, no particle with a specified PDG ID has been found
    return nullptr;
}


GenParticle::collection_t const &GenParticle::GetDaughters() const
{
    return daughters;
}


GenParticle const *GenParticle::GetFirstMother() const
{
    if (mothers.size() == 0)
        return nullptr;
    else
        return mothers.front();
}


int GenParticle::GetFirstMotherPdgId() const
{
    if (mothers.size() == 0)
        return 0;
    else
        return mothers.front()->GetPdgId();
}


GenParticle::collection_t const &GenParticle::GetMothers() const
{
    return mothers;
}


int GenParticle::GetPdgId() const
{
    return pdgId;
}


void GenParticle::SetPdgId(int pdgId_)
{
    pdgId = pdgId_;
}


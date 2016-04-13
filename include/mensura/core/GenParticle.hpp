#pragma once

#include <mensura/core/PhysicsObjects.hpp>

#include <list>
#include <initializer_list>


/**
 * \class GenParticle
 * \brief Describes a generator-level particle
 * 
 * A particle is described by its four-momentum and PDG ID codes. It also stores collections of
 * (non-owning) pointers to its mothers and daughters.
 */
class GenParticle: public Candidate
{
public:
    /// Type of the container to store mothers and daughters
    typedef std::list<GenParticle const *> collection_t;

public:
    /// Default constructor
    GenParticle() noexcept;
    
    /// Constructor with four-momentum and PDG ID code
    GenParticle(TLorentzVector const &p4, int pdgId = 0) noexcept;
    
    /// Default copy constructor
    GenParticle(GenParticle const &) = default;
    
    /// Default move constructor
    GenParticle(GenParticle &&) = default;

public:
    /// Adds a daughter particle
    void AddDaughter(GenParticle const *p);
    
    /// Adds a mother particle
    void AddMother(GenParticle const *p);
    
    /**
     * \brief Returns the pointer to the first daughter that matches one of the given PDG ID codes
     * 
     * The sign of the code is taken in to account. Returns null pointer of no match is found.
     */
    GenParticle const *FindFirstDaughter(std::initializer_list<int> const &pdgIds) const;
    
    /**
     * \brief Recursively looks for a daughter with one of the specified PDG ID codes and returns
     * the pointer to it
     * 
     * The sign of the code is taken into account. This method is called recursively for all
     * daughters until the match is found. If no descendants have the given PDG ID code, a null
     * pointer is returned.
     */
     GenParticle const *FindFirstDaughterRecursive(std::initializer_list<int> const &pdgIds)
      const;
    
    /// Returns the collection of daughter particles
    collection_t const &GetDaughters() const;
    
    /// A short-cut to access the first mother
    GenParticle const *GetFirstMother() const;
    
    /**
     * \brief A short-cut to access PDG ID code of the first mother
     * 
     * If the particle has no mothers, zero is returned.
     */
    int GetFirstMotherPdgId() const;
    
    /// Returns the collection of mother particles
    collection_t const &GetMothers() const;
    
    /// Returns the PDG ID code
    int GetPdgId() const;
    
    /// Sets the PDG ID code
    void SetPdgId(int pdgId_);
    
private:
    /// PDG ID code
    int pdgId;
    
    /// Non-owning pointers to mothers of the particle
    collection_t mothers;
    
    /// Non-owning pointers to daughters of the particle
    collection_t daughters;
};

/**
 * \file GenParticle.hpp
 * \author Andrey Popov
 * 
 * The module implements description of a generator-level particle.
 */

#pragma once

#include <PhysicsObjects.hpp>

#include <list>
#include <initializer_list>


/**
 * \class GenParticle
 * \brief Describes a generator-level particle
 */
class GenParticle: public Candidate
{
    public:
        /// Type of the container to store the mothers and daughters
        typedef std::list<GenParticle const *> collection_t;
    
    public:
        /// Default constructor
        GenParticle();
        
        /// Constructor with 4-momentum and PDG ID
        GenParticle(TLorentzVector const &p4_, int pdgId_ = 0);
    
    public:
        /// Sets the PDG ID
        void SetPdgId(int pdgId_);
        
        /// Adds a mother particle
        void AddMother(GenParticle const *p);
        
        /// Adds a daughter particle
        void AddDaughter(GenParticle const *p);
        
        /// Returns the PDG ID
        int GetPdgId() const;
        
        /// Returns the collection of mother particles
        collection_t const &GetMothers() const;
        
        /// Returns the collection of daughter particles
        collection_t const &GetDaughters() const;
        
        /// Returns the pointer to the first daughter with one of the given PDG ID
        GenParticle const *FindFirstDaughter(std::initializer_list<int> const &pdgIds) const;
        
        /**
         * \brief Recursively looks for a daughter with one of the specified PDG ID and returns
         * the pointer to it
         */
         GenParticle const *FindFirstDaughterRecursive(std::initializer_list<int> const &pdgIds)
          const;
    
    private:
        int pdgId;  ///< The PDG ID code
        collection_t mothers;  ///< Mothers of the particle
        collection_t daughters;  ///< Daughters of the particle
};
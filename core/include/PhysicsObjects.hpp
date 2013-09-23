/**
 * \file PhysicsObjects.hpp
 * \author Andrey Popov
 * 
 * The module defines a set of aggregate types to represent the high-level physical objects.
 */

#pragma once

#include <TLorentzVector.h>


/**
 * \class Candidate
 * \brief Represents a general object with a 4-momentum
 */
class Candidate
{
    public:
        /// Default constructor
        Candidate();
        
        /// Constructor from a 4-momentum
        Candidate(TLorentzVector const &p4_);
    
    public:
        /// Sets the 4-momentum
        void SetP4(TLorentzVector const &p4_);
        
        /// Sets the 4-momentum
        void SetPtEtaPhiM(double pt, double eta, double phi, double mass);
        
        /// Sets the 4-momentum
        void SetPxPyPzE(double px, double py, double pz, double E);
        
        /// The 4-momentum
        TLorentzVector const &P4() const;
        
        /// Transverse momentum
        double Pt() const;
        
        /// Pseudorapidity
        double Eta() const;
        
        /// Azimuthal angle
        double Phi() const;
        
        /// Mass
        double M() const;
        
        /// Ordering operator
        bool operator<(Candidate const &rhs) const;
    
    private:
        TLorentzVector p4;  ///< The 4-momentum
};


/**
 * \class Lepton
 * \brief Represents an electrically-charged lepton
 */
class Lepton: public Candidate
{
    public:
        /// Flavour of the lepton
        enum class Flavour
        {
            Unknown,
            Electron,
            Muon,
            Tau
        };
    
    public:
        /// Default constructor
        Lepton();
        
        /// Constructor with the flavour and the 4-momentum
        Lepton(Flavour flavour_, TLorentzVector const &p4);
    
    public:
        /// Sets the relative isolation
        void SetRelIso(double relIso_);
        
        /// Sets the impact-parameter
        void SetDB(double dB_);
        
        /// Sets the electric charge
        void SetCharge(int charge_);
        
        /// Gets the flavour
        Flavour GetFlavour() const;
        
        /// Gets the relative isolation
        double RelIso() const;
        
        /// Gets the impact parameter
        double DB() const;
        
        /// Gets the electric charge
        int Charge() const;
    
    private:
        Flavour flavour;  ///< The flavour
        double relIso;  ///< The relative isolation
        double dB;  ///< The impact parameter of the associated track
        int charge;  ///< The electric charge
};


/**
 * \class Jet
 * \brief Represents a jet
 */
class Jet: public Candidate
{
    public:
        /// Default constructor
        Jet();
        
        /// Constuctor with the 4-momentum
        Jet(TLorentzVector const &p4);
    
    public:
        /// Sets the values of the b-tagging discriminators
        void SetBTags(double CSV, double JP, double TCHP);
        
        /// Sets the value of the CSV b-tagging discriminator
        void SetCSV(double CSV);
        
        /// Sets the value of the JP b-tagging discriminator
        void SetJP(double JP);
        
        /// Sets the value of the TCHP b-tagging discriminator
        void SetTCHP(double TCHP);
        
        /// Sets the parent's PDF ID
        void SetParentID(int pdgID);
        
        /// Sets the electric charge
        void SetCharge(double charge);
        
        /// Sets jet pull angle
        void SetPullAngle(double pullAngle);
        
        /// Gets the value of the CSV b-tagging discriminator
        double CSV() const;
        
        /// Gets the value of the JP b-tagging discriminator
        double JP() const;
        
        /// Gets the value of the TCHP b-tagging discriminator
        double TCHP() const;
        
        /// Gets the parent's PDG ID
        int GetParentID() const;
        
        /// Gets the electric charge
        double Charge() const;
        
        /// Gets the pull angle
        double GetPullAngle() const;
    
    private:
        double CSVValue;   ///< CSV b-tagging discriminator
        double JPValue;    ///< JP b-tagging discriminator
        double TCHPValue;  ///< TCHP b-tagging discriminator
        int parentPDGID;  ///< PDG ID of the parent
        double charge;  ///< Electric charge
        double pullAngle;  ///< "Pull angle" (characterises the colour flow)
};


/**
 * \class GenJet
 * \brief Represents a generator-level jet
 */
class GenJet: public Candidate
{
    public:
        /// Default constructor
        GenJet();
        
        /// Constructor from a four-momentum
        GenJet(TLorentzVector const &p4);
    
    public:
        /// Sets multipliticy of b and c quarks with status 2 near the jet
        void SetMultiplicities(unsigned bMult, unsigned cMult);
        
        /// Returns multiplicity of b quarks
        unsigned GetBMultiplicity() const;
        
        /// Returns multiplicity of c quarks
        unsigned GetCMultiplicity() const;
    
    private:
        /// Number of b quarks with status 2 near the jet
        unsigned bMult;
        
        /// Number of c quarks with status 2 near the jet
        unsigned cMult;
};

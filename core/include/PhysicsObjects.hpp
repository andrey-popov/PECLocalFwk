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
    Candidate() noexcept;
    
    /// Constructor from a 4-momentum
    Candidate(TLorentzVector const &p4_) noexcept;

public:
    /// Sets the 4-momentum
    void SetP4(TLorentzVector const &p4_) noexcept;
    
    /// Sets the 4-momentum
    void SetPtEtaPhiM(double pt, double eta, double phi, double mass) noexcept;
    
    /// Sets the 4-momentum
    void SetPxPyPzE(double px, double py, double pz, double E) noexcept;
    
    /// The 4-momentum
    TLorentzVector const &P4() const noexcept;
    
    /// Transverse momentum
    double Pt() const noexcept;
    
    /// Pseudorapidity
    double Eta() const noexcept;
    
    /// Azimuthal angle
    double Phi() const noexcept;
    
    /// Mass
    double M() const noexcept;
    
    /// Ordering operator
    bool operator<(Candidate const &rhs) const noexcept;

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
    Lepton() noexcept;
    
    /// Constructor with the flavour and the 4-momentum
    Lepton(Flavour flavour_, TLorentzVector const &p4) noexcept;

public:
    /// Sets the relative isolation
    void SetRelIso(double relIso_) noexcept;
    
    /// Sets the impact-parameter
    void SetDB(double dB_) noexcept;
    
    /// Sets the electric charge
    void SetCharge(int charge_) noexcept;
    
    /// Gets the flavour
    Flavour GetFlavour() const noexcept;
    
    /// Gets the relative isolation
    double RelIso() const noexcept;
    
    /// Gets the impact parameter
    double DB() const noexcept;
    
    /// Gets the electric charge
    int Charge() const noexcept;

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
    Jet() noexcept;
    
    /// Constuctor with the 4-momentum
    Jet(TLorentzVector const &p4) noexcept;

public:
    /// Sets the values of the b-tagging discriminators
    void SetBTags(double CSV, double JP, double TCHP) noexcept;
    
    /// Sets the value of the CSV b-tagging discriminator
    void SetCSV(double CSV) noexcept;
    
    /// Sets the value of the JP b-tagging discriminator
    void SetJP(double JP) noexcept;
    
    /// Sets the value of the TCHP b-tagging discriminator
    void SetTCHP(double TCHP) noexcept;
    
    /// Sets the parent's PDF ID
    void SetParentID(int pdgID) noexcept;
    
    /// Sets the electric charge
    void SetCharge(double charge) noexcept;
    
    /// Sets jet pull angle
    void SetPullAngle(double pullAngle) noexcept;
    
    /// Gets the value of the CSV b-tagging discriminator
    double CSV() const noexcept;
    
    /// Gets the value of the JP b-tagging discriminator
    double JP() const noexcept;
    
    /// Gets the value of the TCHP b-tagging discriminator
    double TCHP() const noexcept;
    
    /// Gets the parent's PDG ID
    int GetParentID() const noexcept;
    
    /// Gets the electric charge
    double Charge() const noexcept;
    
    /// Gets the pull angle
    double GetPullAngle() const noexcept;

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
    GenJet() noexcept;
    
    /// Constructor from a four-momentum
    GenJet(TLorentzVector const &p4) noexcept;

public:
    /// Sets multipliticy of b and c quarks with status 2 near the jet
    void SetMultiplicities(unsigned bMult, unsigned cMult) noexcept;
    
    /// Returns multiplicity of b quarks
    unsigned GetBMultiplicity() const noexcept;
    
    /// Returns multiplicity of c quarks
    unsigned GetCMultiplicity() const noexcept;

private:
    /// Number of b quarks with status 2 near the jet
    unsigned bMult;
    
    /// Number of c quarks with status 2 near the jet
    unsigned cMult;
};


/**
 * \class ShowerParton
 * \brief Describes a parton in parton shower
 */
class ShowerParton: public Candidate
{
public:
    /// Describe the origin of the parton
    enum class Origin
    {
        Unknown,  ///< Not specified
        ISR,      ///< Can be traced down to initial lines
        FSR,      ///< Can be traced down to final lines
        Proton    ///< An immediate daughter of a beam particle
    };
    
public:
    /// Constructor with no parameters
    ShowerParton() noexcept;
    
    /// Constructor from a four-momentum and a code of origin
    ShowerParton(TLorentzVector const &p4, Origin origin = Origin::Unknown) noexcept;
    
public:
    /// Sets the origin
    void SetOrigin(Origin origin) noexcept;
    
    /// Returns the origin
    Origin GetOrigin() const noexcept;
    
private:
    /// Origin of the parton
    Origin origin;
};

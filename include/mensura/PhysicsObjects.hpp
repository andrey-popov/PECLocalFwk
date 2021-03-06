/**
 * \file PhysicsObjects.hpp
 * 
 * The module defines a set of aggregate types to represent the high-level physical objects.
 */

#pragma once

#include <mensura/BTagger.hpp>

#include <TLorentzVector.h>

#include <array>
#include <map>
#include <unordered_map>


// Forward declarations
class GenJet;


/**
 * \class Candidate
 * \brief Represents a general object with a four-momentum
 * 
 * An object of this class can also contain an arbitrary number of real- and integer-valued
 * properties defined by user.
 */
class Candidate
{
public:
    /// Default constructor
    Candidate() noexcept;
    
    /// Constructor from a 4-momentum
    Candidate(TLorentzVector const &p4_) noexcept;

public:
    /// Sets four-momentum
    void SetP4(TLorentzVector const &p4_) noexcept;
    
    /// Sets four-momentum
    void SetPtEtaPhiM(double pt, double eta, double phi, double mass) noexcept;
    
    /// Sets four-momentum
    void SetPxPyPzE(double px, double py, double pz, double E) noexcept;
    
    /// Sets or changes value of a user-defined real-valued property
    void SetUserFloat(std::string const &label, double value);
    
    /// Sets or changes value of a user-defined integer-valued property
    void SetUserInt(std::string const &label, long value);
    
    /// Four-momentum
    TLorentzVector const &P4() const noexcept;
    
    /// Transverse momentum, GeV/c
    double Pt() const noexcept;
    
    /// Pseudorapidity
    double Eta() const noexcept;
    
    /// Azimuthal angle
    double Phi() const noexcept;
    
    /// Mass, GeV/c^2
    double M() const noexcept;
    
    /// Energy, GeV
    double E() const noexcept;
    
    /**
     * \brief Returns value of the user-defined property with the given label
     * 
     * An exception of thrown if no property with the given label is defined.
     */
    double UserFloat(std::string const &label) const;
    
    /**
     * \brief Returns value of the user-defined property with the given label
     * 
     * An exception of thrown if no property with the given label is defined.
     */
    long UserInt(std::string const &label) const;
    
    /// Ordering operator
    bool operator<(Candidate const &rhs) const noexcept;

private:
    /// Four-momentum
    TLorentzVector p4;
    
    /// Map implementing user-defined real-valued properties
    std::unordered_map<std::string, double> userFloats;
    
    /// Map implementing user-defined integer-valued properties
    std::unordered_map<std::string, long> userInts;
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
    
    /// Sets the electric charge
    void SetCharge(int charge_) noexcept;
    
    /// Gets the flavour
    Flavour GetFlavour() const noexcept;
    
    /// Gets the relative isolation
    double RelIso() const noexcept;
    
    /// Gets the electric charge
    int Charge() const noexcept;

private:
    Flavour flavour;  ///< The flavour
    double relIso;  ///< The relative isolation
    int charge;  ///< The electric charge
};


/**
 * \class Jet
 * \brief Represents a jet
 * 
 * The four-momentum inherited from the base class is fully corrected. An object stores also a
 * scale factor to reproduce raw momentum.
 */
class Jet: public Candidate
{
public:
    /**
     * \brief Definitions of jet flavour
     * 
     * Detailed descriptions of the definitions are provided in [1].
     * [1] https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuideBTagMCTools
     */
    enum class FlavourType: unsigned
    {
        /// Jet clustering with ghost b- and c-hadrons
        Hadron = 0,
        
        /// Jet clustering with ghost partons
        Parton = 1,
        
        /// Matching to partons in final state of matrix element (the "physics" definition)
        ME = 2
    };
    
public:
    /// Default constructor
    Jet() noexcept;
    
    /// Constuctor from the fully-corrected four-momentum
    Jet(TLorentzVector const &correcteP4) noexcept;
    
    /**
     * \brief Constructor from the raw four-momentum and total correction scale factor
     * 
     * The scale factor includes full JEC and, in case of simulation, JER. A fully-corrected
     * momentum is calculated as rawP4 * corrSF.
     */
    Jet(TLorentzVector const &rawP4, double corrSF) noexcept;

public:
    /**
     * \brief Sets jet corrected and raw momentum
     * 
     * The first argument is a fully-corrected four-momentum. The second argument is a scale factor
     * to calculate the raw momentum from it.
     */
    void SetCorrectedP4(TLorentzVector const &correctedP4, double rawMomentumSF) noexcept;
    
    /// Sets value of a b-tagging discriminator
    void SetBTag(BTagger::Algorithm algo, double value) noexcept;
    
    /// Sets "hadron" flavour of the jet
    [[deprecated("Use Jet::SetFlavour instead")]]
    void SetParentID(int pdgID) noexcept;
    
    /// Sets the electric charge
    void SetCharge(double charge) noexcept;
    
    /// Sets jet flavour according to the given definition
    void SetFlavour(FlavourType type, int flavour);
    
    /// Sets jet pull angle
    void SetPullAngle(double pullAngle) noexcept;
    
    /// Sets jet area
    void SetArea(double area) noexcept;
    
    /// Sets matched generator-level jet
    void SetMatchedGenJet(GenJet const *matchedJet) noexcept;
    
    /// Sets value of pile-up ID discriminator
    void SetPileUpID(double puDiscriminator);
    
    /// Returns raw momentum
    TLorentzVector RawP4() const noexcept;
    
    /// Returns value of the requested b-tagging discriminator
    double BTag(BTagger::Algorithm algo) const;
    
    /// Gets the value of the CSV b-tagging discriminator
    [[deprecated("Use Jet::BTag instead")]]
    double CSV() const;
    
    /// Gets the value of the JP b-tagging discriminator
    [[deprecated("Use Jet::BTag instead")]]
    double JP() const;
    
    /**
     * \brief Returns jet flavour according to the requested definition
     * 
     * A value of zero is returned if flavour of the jet is not identified or has never been
     * filled.
     */
    int Flavour(FlavourType type = FlavourType::Hadron) const;
    
    /// Returns "hadron" flavour of the jet
    [[deprecated("Use Jet::Flavour instead")]]
    int GetParentID() const noexcept;
    
    /// Gets the electric charge
    double Charge() const noexcept;
    
    /// Gets the pull angle
    double GetPullAngle() const noexcept;
    
    /// Returns jet area
    double Area() const noexcept;
    
    /**
     * \brief Returns matched generator-level jet (if applicable)
     * 
     * If no jet is matched or generator-level jets are not available, returns a null pointer.
     */
    GenJet const *MatchedGenJet() const noexcept;
    
    /// Returns value of pile-up ID discriminator
    double PileUpID() const;

private:
    /// A scale factor to build raw four-momentum
    double rawMomentumSF;
    
    /// Values of b-tagging discriminators
    std::map<BTagger::Algorithm, double> bTagValues;
    
    /**
     * \brief Jet flavours
     * 
     * The order is the same as in the enumeration FlavourType.
     */
    std::array<int, 3> flavours;
    
    /// Electric charge
    double charge;
    
    /// "Pull angle" (characterises the colour flow)
    double pullAngle;
    
    /// Jet area
    double area;
    
    /// Pile-up ID discriminator
    double puDiscriminator;
    
    /// Pointer to matched generator-level jet
    GenJet const *matchedGenJet;
};


/// An alias for MET
typedef Candidate MET;


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
    
    /// Constructor from a four-momentum, PDG ID, and a code of origin
    ShowerParton(TLorentzVector const &p4, int pdgId, Origin origin = Origin::Unknown) noexcept;
    
    /**
     * \brief Constructor from three-momentum, PDG ID, and a code of origin
     * 
     * The mass needed to build the four-momentum is calculated from the PDG ID using the GuessMass
     * method.
     */
    ShowerParton(double pt, double eta, double phi, int pdgId, Origin origin = Origin::Unknown)
     noexcept;
    
public:
    /// Sets PDG ID of the parton
    void SetPdgId(int pdgId) noexcept;
    
    /// Returns PDG ID of the parton
    int GetPdgId() const noexcept;
    
    /// Sets the origin
    void SetOrigin(Origin origin) noexcept;
    
    /// Returns the origin
    Origin GetOrigin() const noexcept;
    
private:
    /**
     * \brief Tries to return mass of a particle with the given PDG ID
     * 
     * For particles other that quarks sets the mass to zero.
     */
    static double GuessMass(int pdgId) noexcept;
    
private:
    /// PDG ID of the parton
    int pdgId;
    
    /// Origin of the parton
    Origin origin;
};

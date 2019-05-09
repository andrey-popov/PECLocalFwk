#include <mensura/PhysicsObjects.hpp>

#include <limits>
#include <sstream>
#include <stdexcept>


using namespace std;


// Methods of class Candidate
Candidate::Candidate() noexcept
{}


Candidate::Candidate(TLorentzVector const &p4_) noexcept:
    p4(p4_)
{}


void Candidate::SetP4(TLorentzVector const &p4_) noexcept
{
    p4 = p4_;
}


void Candidate::SetPtEtaPhiM(double pt, double eta, double phi, double mass) noexcept
{
    p4.SetPtEtaPhiM(pt, eta, phi, mass);
}


void Candidate::SetPxPyPzE(double px, double py, double pz, double E) noexcept
{
    p4.SetPxPyPzE(px, py, pz, E);
}


void Candidate::SetUserFloat(std::string const &label, double value)
{
    userFloats[label] = value;
}


void Candidate::SetUserInt(std::string const &label, long value)
{
    userInts[label] = value;
}


TLorentzVector const &Candidate::P4() const noexcept
{
    return p4;
}


double Candidate::Pt() const noexcept
{
    return p4.Pt();
}


double Candidate::Eta() const noexcept
{
    return p4.Eta();
}


double Candidate::Phi() const noexcept
{
    return p4.Phi();
}


double Candidate::M() const noexcept
{
    return p4.M();
}


double Candidate::E() const noexcept
{
    return p4.E();
}


double Candidate::UserFloat(std::string const &label) const
{
    auto const res = userFloats.find(label);
    
    if (res == userFloats.end())
    {
        std::ostringstream ost;
        ost << "Candidate::UserFloat: Real-valued property with label \"" << label <<
          "\" is not defined.";
        throw std::out_of_range(ost.str());
    }
    else
        return res->second;
}


long Candidate::UserInt(std::string const &label) const
{
    auto const res = userInts.find(label);
    
    if (res == userInts.end())
    {
        std::ostringstream ost;
        ost << "Candidate::UserInt: Integer-valued property with label \"" << label <<
          "\" is not defined.";
        throw std::out_of_range(ost.str());
    }
    else
        return res->second;
}


bool Candidate::operator<(Candidate const &rhs) const noexcept
{
    return (p4.Pt() < rhs.p4.Pt());
}


// Methods of class Lepton
Lepton::Lepton() noexcept:
    Candidate(),
    flavour(Flavour::Unknown),
    relIso(-1.), charge(0)
{}


Lepton::Lepton(Lepton::Flavour flavour_, TLorentzVector const &p4) noexcept:
    Candidate(p4),
    flavour(flavour_),
    relIso(-1.), charge(0)
{}


void Lepton::SetRelIso(double relIso_) noexcept
{
    relIso = relIso_;
}


void Lepton::SetCharge(int charge_) noexcept
{
    charge = charge_;
}


Lepton::Flavour Lepton::GetFlavour() const noexcept
{
    return flavour;
}


double Lepton::RelIso() const noexcept
{
    return relIso;
}


int Lepton::Charge() const noexcept
{
    return charge;
}


// Methods of class Jet
Jet::Jet() noexcept:
    Candidate(),
    rawMomentumSF(0.),
    flavours{0, 0, 0},
    charge(-10.), pullAngle(-10.),
    puDiscriminator(0.),
    matchedGenJet(nullptr)
{}


Jet::Jet(TLorentzVector const &correctedP4) noexcept:
    Candidate(correctedP4),
    rawMomentumSF(0.),
    flavours{0, 0, 0},
    charge(-10.), pullAngle(-10.),
    puDiscriminator(0.),
    matchedGenJet(nullptr)
{}


Jet::Jet(TLorentzVector const &rawP4, double corrSF) noexcept:
    Candidate(rawP4 * corrSF),
    rawMomentumSF(1. / corrSF),
    flavours{0, 0, 0},
    charge(-10.), pullAngle(-10.),
    puDiscriminator(0.),
    matchedGenJet(nullptr)
{}


void Jet::SetCorrectedP4(TLorentzVector const &correctedP4, double rawMomentumSF_) noexcept
{
    SetP4(correctedP4);
    rawMomentumSF = rawMomentumSF_;
}


void Jet::SetBTag(BTagger::Algorithm algo, double value) noexcept
{
    bTagValues[algo] = value;
}


void Jet::SetParentID(int pdgID) noexcept
{
    SetFlavour(FlavourType::Hadron, pdgID);
}


void Jet::SetCharge(double charge_) noexcept
{
    charge = charge_;
}


void Jet::SetFlavour(Jet::FlavourType type, int flavour)
{
    try
    {
        flavours.at(unsigned(type)) = flavour;
    }
    catch (std::out_of_range const &)
    {
        std::ostringstream message;
        message << "Jet::SetFlavour: Unknown jet flavour (" << unsigned(type) << ") is given.";
        throw std::runtime_error(message.str());
    }
}


void Jet::SetPullAngle(double pullAngle_) noexcept
{
    pullAngle = pullAngle_;
}


void Jet::SetArea(double area_) noexcept
{
    area = area_;
}


void Jet::SetMatchedGenJet(GenJet const *matchedJet) noexcept
{
    matchedGenJet = matchedJet;
}

void Jet::SetPileUpID(double puDiscriminator_)
{
    puDiscriminator = puDiscriminator_;
}


TLorentzVector Jet::RawP4() const noexcept
{
    return P4() * rawMomentumSF;
}


double Jet::BTag(BTagger::Algorithm algo) const
{
    auto const res = bTagValues.find(algo);
    
    if (res == bTagValues.end())
        throw std::runtime_error("Jet::BTag: No value of b-tagging discriminator is available "s +
          "for algorithm " + BTagger::AlgorithmToTextCode(algo) + ".");
    
    return res->second;
}


double Jet::CSV() const
{
    return BTag(BTagger::Algorithm::CSV);
}


double Jet::JP() const
{
    return BTag(BTagger::Algorithm::JP);
}


int Jet::Flavour(Jet::FlavourType type /*= FlavourType::Hadron*/) const
{
    try
    {
        return flavours.at(unsigned(type));
    }
    catch (std::out_of_range const &)
    {
        std::ostringstream message;
        message << "Jet::Flavour: Unknown jet flavour (" << unsigned(type) << ") is given.";
        throw std::runtime_error(message.str());
    }
}


int Jet::GetParentID() const noexcept
{
    return Flavour(FlavourType::Hadron);
}


double Jet::Charge() const noexcept
{
    return charge;
}


double Jet::GetPullAngle() const noexcept
{
    return pullAngle;
}


double Jet::Area() const noexcept
{
    return area;
}


GenJet const *Jet::MatchedGenJet() const noexcept
{
    return matchedGenJet;
}


double Jet::PileUpID() const
{
    return puDiscriminator;
}


// Methods of class GenJet
GenJet::GenJet() noexcept:
    Candidate(),
    bMult(0), cMult(0)
{}


GenJet::GenJet(TLorentzVector const &p4) noexcept:
    Candidate(p4),
    bMult(0), cMult(0)
{}


void GenJet::SetMultiplicities(unsigned bMult_, unsigned cMult_) noexcept
{
    bMult = bMult_;
    cMult = cMult_;
}


unsigned GenJet::GetBMultiplicity() const noexcept
{
    return bMult;
}


unsigned GenJet::GetCMultiplicity() const noexcept
{
    return cMult;
}


// Methods of the ShowerParton class
ShowerParton::ShowerParton() noexcept:
    Candidate(),
    pdgId(0), origin(Origin::Unknown)
{}


ShowerParton::ShowerParton(TLorentzVector const &p4_, int pdgId_,
 Origin origin_ /*= Origin::Unknown*/) noexcept:
    Candidate(p4_),
    pdgId(pdgId_), origin(origin_)
{}


ShowerParton::ShowerParton(double pt, double eta, double phi, int pdgId_,
 Origin origin_ /*= Origin::Unknown*/) noexcept:
    Candidate(),
    pdgId(pdgId_), origin(origin_)
{
    SetPtEtaPhiM(pt, eta, phi, GuessMass(pdgId));
}


void ShowerParton::SetOrigin(Origin origin_) noexcept
{
    origin = origin_;
}


ShowerParton::Origin ShowerParton::GetOrigin() const noexcept
{
    return origin;
}


void ShowerParton::SetPdgId(int pdgId_) noexcept
{
    pdgId = pdgId_;
}


int ShowerParton::GetPdgId() const noexcept
{
    return pdgId;
}


double ShowerParton::GuessMass(int pdgId) noexcept
{
    // Masses for s, c, b are set to values used in Pythia in Summer12 datasets
    switch (abs(pdgId))
    {
        case 6:
            return 172.5;
        
        case 5:
            return 4.8;
        
        case 4:
            return 1.5;
        
        case 3:
            return 0.5;
        
        case 2:
            return 0.;
        
        case 1:
            return 0.;
        
        default:
            return 0.;
    }
}

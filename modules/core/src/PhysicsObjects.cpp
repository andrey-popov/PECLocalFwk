#include <mensura/core/PhysicsObjects.hpp>

#include <limits>
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
    parentPDGID(0),
    charge(-10.), pullAngle(-10.),
    matchedGenJet(nullptr)
{}


Jet::Jet(TLorentzVector const &correctedP4) noexcept:
    Candidate(correctedP4),
    rawMomentumSF(0.),
    parentPDGID(0),
    charge(-10.), pullAngle(-10.),
    matchedGenJet(nullptr)
{}


Jet::Jet(TLorentzVector const &rawP4, double corrSF) noexcept:
    Candidate(rawP4 * corrSF),
    rawMomentumSF(1. / corrSF),
    parentPDGID(0),
    charge(-10.), pullAngle(-10.),
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
    parentPDGID = pdgID;
}


void Jet::SetCharge(double charge_) noexcept
{
    charge = charge_;
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


int Jet::GetParentID() const noexcept
{
    return parentPDGID;
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

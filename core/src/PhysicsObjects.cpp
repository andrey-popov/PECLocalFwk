#include <PhysicsObjects.hpp>

#include <limits>


using namespace std;


// Methods of class Candidate
Candidate::Candidate()
{}


Candidate::Candidate(TLorentzVector const &p4_):
    p4(p4_)
{}


void Candidate::SetP4(TLorentzVector const &p4_)
{
    p4 = p4_;
}


void Candidate::SetPtEtaPhiM(double pt, double eta, double phi, double mass)
{
    p4.SetPtEtaPhiM(pt, eta, phi, mass);
}


void Candidate::SetPxPyPzE(double px, double py, double pz, double E)
{
    p4.SetPxPyPzE(px, py, pz, E);
}


TLorentzVector const &Candidate::P4() const
{
    return p4;
}


double Candidate::Pt() const
{
    return p4.Pt();
}


double Candidate::Eta() const
{
    return p4.Eta();
}


double Candidate::Phi() const
{
    return p4.Phi();
}


double Candidate::M() const
{
    return p4.M();
}


bool Candidate::operator<(Candidate const &rhs) const
{
    return (p4.Pt() < rhs.p4.Pt());
}


// Methods of class Lepton
Lepton::Lepton():
    Candidate(),
    flavour(Flavour::Unknown),
    relIso(-1.), dB(0.), charge(0)
{}


Lepton::Lepton(Lepton::Flavour flavour_, TLorentzVector const &p4):
    Candidate(p4),
    flavour(flavour_),
    relIso(-1.), dB(0.), charge(0)
{}


void Lepton::SetRelIso(double relIso_)
{
    relIso = relIso_;
}


void Lepton::SetDB(double dB_)
{
    dB = dB_;
}


void Lepton::SetCharge(int charge_)
{
    charge = charge_;
}


Lepton::Flavour Lepton::GetFlavour() const
{
    return flavour;
}


double Lepton::RelIso() const
{
    return relIso;
}


double Lepton::DB() const
{
    return dB;
}


int Lepton::Charge() const
{
    return charge;
}


// Methods of class Jet
Jet::Jet():
    Candidate(),
    CSVValue(-numeric_limits<double>::infinity()),
    JPValue(-numeric_limits<double>::infinity()),
    TCHPValue(-numeric_limits<double>::infinity()),
    parentPDGID(0),
    charge(-10.), pullAngle(-10.)
{}


Jet::Jet(TLorentzVector const &p4):
    Candidate(p4),
    CSVValue(-numeric_limits<double>::infinity()),
    JPValue(-numeric_limits<double>::infinity()),
    TCHPValue(-numeric_limits<double>::infinity()),
    parentPDGID(0),
    charge(-10.), pullAngle(-10.)
{}


void Jet::SetBTags(double CSV, double JP, double TCHP)
{
    CSVValue = CSV;
    JPValue = JP;
    TCHPValue = TCHP;
}


void Jet::SetCSV(double CSV)
{
    CSVValue = CSV;
}


void Jet::SetJP(double JP)
{
    JPValue = JP;
}


void Jet::SetTCHP(double TCHP)
{
    TCHPValue = TCHP;
}


void Jet::SetParentID(int pdgID)
{
    parentPDGID = pdgID;
}


void Jet::SetCharge(double charge_)
{
    charge = charge_;
}


void Jet::SetPullAngle(double pullAngle_)
{
    pullAngle = pullAngle_;
}


double Jet::CSV() const
{
    return CSVValue;
}


double Jet::JP() const
{
    return JPValue;
}


double Jet::TCHP() const
{
    return TCHPValue;
}


int Jet::GetParentID() const
{
    return parentPDGID;
}


double Jet::Charge() const
{
    return charge;
}


double Jet::GetPullAngle() const
{
    return pullAngle;
}


// Methods of class GenJet
GenJet::GenJet():
    Candidate(),
    bMult(0), cMult(0)
{}


GenJet::GenJet(TLorentzVector const &p4):
    Candidate(p4),
    bMult(0), cMult(0)
{}


void GenJet::SetMultiplicities(unsigned bMult_, unsigned cMult_)
{
    bMult = bMult_;
    cMult = cMult_;
}


unsigned GenJet::GetBMultiplicity() const
{
    return bMult;
}


unsigned GenJet::GetCMultiplicity() const
{
    return cMult;
}


// Methods of the ShowerParton class
ShowerParton::ShowerParton() noexcept:
    Candidate(),
    origin(Origin::Unknown)
{}


ShowerParton::ShowerParton(TLorentzVector const &p4_, Origin origin_ /*= Origin::Unknown*/)
 noexcept:
    Candidate(p4_),
    origin(origin_)
{}


void ShowerParton::SetOrigin(Origin origin_) noexcept
{
    origin = origin_;
}


ShowerParton::Origin ShowerParton::GetOrigin() const noexcept
{
    return origin;
}

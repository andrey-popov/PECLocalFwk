#include <PECFwk/PECReader/Lepton.hpp>

#include <stdexcept>


pec::Lepton::Lepton():
    CandidateWithID(),
    charge(false), relIso(0), dB(0)
{}


pec::Lepton::Lepton(Lepton const &src):
    CandidateWithID(src),
    charge(src.charge), relIso(src.relIso), dB(src.dB)
{}


pec::Lepton &pec::Lepton::operator=(Lepton const &src)
{
    CandidateWithID::operator=(src);
    
    charge = src.charge;
    relIso = src.relIso;
    dB = src.dB;
    
    return *this;
}


pec::Lepton::~Lepton()
{}


void pec::Lepton::Reset()
{
    CandidateWithID::Reset();
    
    charge = false;
    relIso = 0;
    dB = 0;
}


void pec::Lepton::SetCharge(int charge_)
{
    if (charge_ == 0)
        throw std::logic_error("Lepton::SetCharge: The class is meant for charged leptons only.");
    
    charge = (charge_ < 0);
}


void pec::Lepton::SetRelIso(float relIso_)
{
    relIso = relIso_;
}


void pec::Lepton::SetDB(float dB_)
{
    dB = dB_;
}


int pec::Lepton::Charge() const
{
    return ((charge) ? -1 : 1);
}


float pec::Lepton::RelIso() const
{
    return relIso;
}


float pec::Lepton::DB() const
{
    return dB;
}
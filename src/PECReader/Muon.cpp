#include <PECFwk/PECReader/Muon.hpp>


pec::Muon::Muon():
    Lepton()
{}


pec::Muon::Muon(Muon const &src):
    Lepton(src)
{}


pec::Muon &pec::Muon::operator=(Muon const &src)
{
    Lepton::operator=(src);
    
    return *this;
}


void pec::Muon::Reset()
{
    Lepton::Reset();
}

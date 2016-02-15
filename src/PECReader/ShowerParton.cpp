#include <PECFwk/PECReader/ShowerParton.hpp>

#include <stdexcept>


pec::ShowerParton::ShowerParton():
    Candidate(),
    pdgId(0),
    origin(0)
{}


pec::ShowerParton::ShowerParton(ShowerParton const &src):
    Candidate(src),
    pdgId(src.pdgId),
    origin(src.origin)
{}


pec::ShowerParton &pec::ShowerParton::operator=(ShowerParton const &src)
{
    Candidate::operator=(src);
    
    pdgId = src.pdgId;
    origin = src.origin;
    
    return *this;
}


void pec::ShowerParton::Reset()
{
    Candidate::Reset();
    
    pdgId = 0;
    origin = 0;
}


void pec::ShowerParton::SetPdgId(int pdgId_)
{
    if (pdgId_ > 127 or pdgId_ < -128)
        throw std::runtime_error("ShowerParton::SetPdgId: Current implementation allows only one "
         "byte for the PDG ID.");
    
    pdgId = pdgId_;
}


void pec::ShowerParton::SetOrigin(Origin origin_)
{
    origin = UChar_t(origin_);
}


int pec::ShowerParton::PdgId() const
{
    return pdgId;
}


pec::ShowerParton::Origin pec::ShowerParton::GetOrigin() const
{
    return Origin(origin);
}

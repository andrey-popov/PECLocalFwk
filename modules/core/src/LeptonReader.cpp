#include <mensura/core/LeptonReader.hpp>


LeptonReader::LeptonReader(std::string const name /*= "Leptons"*/):
    ReaderPlugin(name)
{}


LeptonReader::~LeptonReader()
{}


std::vector<Lepton> const &LeptonReader::GetLeptons() const
{
    return leptons;
}


std::vector<Lepton> const &LeptonReader::GetLooseLeptons() const
{
    return looseLeptons;
}

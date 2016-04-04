#include <PECFwk/core/GenJetMETReader.hpp>


GenJetMETReader::GenJetMETReader(std::string const name /*= "GenJetMET"*/):
    ReaderPlugin(name)
{}


GenJetMETReader::~GenJetMETReader() noexcept
{}


std::vector<GenJet> const &GenJetMETReader::GetJets() const
{
    return jets;
}


MET const &GenJetMETReader::GetMET() const
{
    return met;
}

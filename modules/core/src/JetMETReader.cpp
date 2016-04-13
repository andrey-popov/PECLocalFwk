#include <mensura/core/JetMETReader.hpp>


JetMETReader::JetMETReader(std::string const name /*= "JetMET"*/):
    ReaderPlugin(name)
{}


JetMETReader::~JetMETReader() noexcept
{}


std::vector<Jet> const &JetMETReader::GetJets() const
{
    return jets;
}


MET const &JetMETReader::GetMET() const
{
    return met;
}

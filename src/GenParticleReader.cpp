#include <mensura/GenParticleReader.hpp>


GenParticleReader::GenParticleReader(std::string const name /*= "GenParticles"*/):
    ReaderPlugin(name)
{}


GenParticleReader::~GenParticleReader()
{}


std::vector<GenParticle> const &GenParticleReader::GetParticles() const
{
    return particles;
}

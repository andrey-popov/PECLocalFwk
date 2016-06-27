#include <mensura/core/GeneratorReader.hpp>

#include <sstream>
#include <stdexcept>


GeneratorReader::GeneratorReader(std::string const name /*= "Generator"*/):
    ReaderPlugin(name)
{}


double GeneratorReader::GetAltWeight(unsigned) const
{
    std::stringstream message;
    message << "GeneratorReader[\"" << GetName() << "\"]::GetAltWeight: Method not implemented.";
    throw std::logic_error(message.str());
}


unsigned GeneratorReader::GetNumAltWeights() const
{
    return 0;
}

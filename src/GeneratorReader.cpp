#include <mensura/GeneratorReader.hpp>

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


std::pair<int, int> GeneratorReader::GetPdfPart() const
{
    std::stringstream message;
    message << "GeneratorReader[\"" << GetName() << "\"]::GetPdfPart: Method not implemented.";
    throw std::logic_error(message.str());
}


std::pair<double, double> GeneratorReader::GetPdfX() const
{
    std::stringstream message;
    message << "GeneratorReader[\"" << GetName() << "\"]::GetPdfX: Method not implemented.";
    throw std::logic_error(message.str());
}


double GeneratorReader::GetScale() const
{
    std::stringstream message;
    message << "GeneratorReader[\"" << GetName() << "\"]::GetScale: Method not implemented.";
    throw std::logic_error(message.str());
}

#include <mensura/PileUpReader.hpp>


PileUpReader::PileUpReader(std::string const name /*= "PileUp"*/):
    ReaderPlugin(name)
{}


PileUpReader::~PileUpReader()
{}


unsigned PileUpReader::GetNumVertices() const
{
    return numVertices;
}


double PileUpReader::GetExpectedPileUp() const
{
    return expectedPileUp;
}


double PileUpReader::GetRho() const
{
    return rho;
}

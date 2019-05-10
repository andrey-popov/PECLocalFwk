#include <mensura/JetResolution.hpp>

#include <mensura/FileInPath.hpp>

#include "external/JERC/JetResolution.hpp" 


JetResolution::JetResolution(std::string const &path):
    jerProvider{new JME::JetResolution(FileInPath::Resolve("JERC", path))}
{}


JetResolution::~JetResolution() noexcept
{}


double JetResolution::operator()(double corrPt, double eta, double rho) const
{
    return jerProvider->getResolution({{JME::Binning::JetPt, corrPt},
        {JME::Binning::JetEta, eta}, {JME::Binning::Rho, rho}});
}


#pragma once

#include <memory>
#include <string>


namespace JME {
    class JetResolution;
};


/// An interface to access jet pt resolution
class JetResolution
{
public:
    /**
     * \brief Constructs from a text file that describes jet pt resolution
     *
     * The file must follow the standard format adopted in the JERC group. The path to it is
     * resolved using FileInPath under a directory "JERC".
     *
     * The pt resultion is described in file jerFile, which must follow the standard format adopted
     * in the JERC group. The path to the file is resolved using the FileInPath service under a
     * subdirectory "JERC". The factor jerPtFactor is applied to the resolution before the
     * comparison.
     */
    JetResolution(std::string const &path);

    ~JetResolution() noexcept;

    /**
     * \brief Returns pt resolution in simulation for a jet with given pt and eta
     *
     * The result also depends on the median angular pt density (rho).
     */
    double operator()(double corrPt, double eta, double rho) const;

private:
    /// Object that reads the resolution under the hood
    std::unique_ptr<JME::JetResolution> jerProvider;
};


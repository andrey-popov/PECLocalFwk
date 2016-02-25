#pragma once

#include <PECFwk/core/Service.hpp>

#include <PECFwk/core/BTagger.hpp>
#include <PECFwk/external/BTagCalibration/BTagEntry.hpp>

#include <map>
#include <memory>
#include <string>
#include <utility>


class BTagCalibration;
class BTagCalibrationReader;
class Jet;


/**
 * \class BTagSFService
 * \brief Service to retrieve b-tagging scale factors
 * 
 * This service reads b-tagging scale factor from standard CSV files. It interfaces code provided
 * by BTV POG [1], which is included as the package external/BTagCalibration.
 * 
 * A single instance of this service retrives scale factors for a single b-tagging configuration
 * (i.e. algorithm and working point). After an instance is created, the user must specify with the
 * method SetMeasurement which "measurements" should be used for which jet flavours.
 * 
 * All objects from external/BTagCalibration are shared among all clones of this service. They are
 * believed to be thread-safe.
 * 
 * [1] https://twiki.cern.ch/twiki/bin/view/CMS/BTagCalibration?rev=31
 */
class BTagSFService: public Service
{
public:
    /// Jet flavours supported for scale factors
    enum class Flavour
    {
        Bottom,
        Charm,
        Light
    };
    
    /// Supported systematic variations
    enum class Variation
    {
        Nominal,
        Up,
        Down
    };
    
private:
    /// A simple structure to aggregate various scale factor readers for the same jet flavour
    struct ReaderSystGroup
    {
        /// Jet flavour translated into the format of external/BTagCalibration
        BTagEntry::JetFlavor translatedFlavour;
        
        /// Scale factor readers for various systematic variations
        std::map<Variation, std::unique_ptr<BTagCalibrationReader>> readers;
    };
    
public:
    /**
     * \brief Creates a service with the given name
     * 
     * Scale factors are read from the given CSV file. Its name is resolved with the help of class
     * FileInPath, adding a postfix "BTag/" to the standard location. The last optional argument
     * indicates whether the service should read also scale factors with systematical variations in
     * addition to nominal ones.
     */
    BTagSFService(std::string const &name, BTagger const &bTagger, std::string const &fileName,
      bool readSystematics = true);
    
    /// A short-cut for the above version with a default name "BTagSF"
    BTagSFService(BTagger const &bTagger, std::string const &fileName,
      bool readSystematics = true);
    
    /// Copy constructor
    BTagSFService(BTagSFService const &src) noexcept;
    
    /// Default move constructor
    BTagSFService(BTagSFService &&) = default;
    
    /// Assignment operator is deleted
    BTagSFService &operator=(BTagSFService const &) = delete;
    
    /// Trivial virtual destructor
    virtual ~BTagSFService() noexcept;
    
public:
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Service.
     */
    virtual Service *Clone() const override;
    
    /**
     * \brief Calculates b-tagging scale factor for given jet momentum and flavour
     * 
     * The computation is performed with the help of package external/BTagCalibration. If given pt
     * is outside of the supported range, the uncertainty is doubled. For pt < 20 GeV a zero scale
     * factor is always returned.
     */
    double GetScaleFactor(double pt, double eta, int flavour, Variation var = Variation::Nominal)
      const;
    
    /// Short-cut for the overloaded version above
    double GetScaleFactor(Jet const &jet, Variation var = Variation::Nominal) const;
    
    /**
     * \brief Specifies what measurement should be used for the given flavour
     * 
     * Usually a CSV file contains scale factors obtained with multiple "measurements", typically
     * different ones for different jet flavours. Labels identifying measurements must be specified
     * before scale factors can be computed.
     */
    void SetMeasurement(Flavour flavour, std::string const &label);
    
private:
    /// Translates given working point and reads the CSV file with scale factors
    void Initialize(BTagger const &bTagger, std::string const &fileName);
    
private:
    /// Specifies whether the service should be able to provide systematic variations
    bool readSystematics;
    
    /**
     * \brief Selected working point of the b-tagging algorithm
     * 
     * Translated into the format used in package external/BTagCalibration.
     */
    BTagEntry::OperatingPoint translatedWP;
    
    /**
     * \brief An object that reads CSV files with scale factors
     * 
     * The object is shared among all clones of this.
     */
    std::shared_ptr<BTagCalibration> bTagCalibration;
    
    /**
     * \brief Objects that compute scale factors, organized by jet flavour
     * 
     * The reader objects are shared among all clones of this.
     */
    std::map<Flavour, std::shared_ptr<ReaderSystGroup>> sfReaders;
};

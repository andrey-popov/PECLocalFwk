#include <mensura/extensions/BTagSFService.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/core/PhysicsObjects.hpp>
#include <mensura/external/BTagCalibration/BTagCalibration.hpp>
#include <mensura/external/BTagCalibration/BTagCalibrationReader.hpp>

#include <cmath>
#include <stdexcept>
#include <sstream>


using namespace std::literals::string_literals;


BTagSFService::BTagSFService(std::string const &name, BTagger const &bTagger,
  std::string const &fileName, bool readSystematics_ /*= true*/):
    Service(name),
    readSystematics(readSystematics_)
{
    Initialize(bTagger, fileName);
}


BTagSFService::BTagSFService(BTagger const &bTagger, std::string const &fileName,
  bool readSystematics_ /*= true*/):
    Service("BTagSF"),
    readSystematics(readSystematics_)
{
    Initialize(bTagger, fileName);
}


BTagSFService::BTagSFService(BTagSFService const &src) noexcept:
    Service(src),
    readSystematics(src.readSystematics),
    translatedWP(src.translatedWP),
    bTagCalibration(src.bTagCalibration),  // shared
    sfReaders(src.sfReaders)  // reader groups are shared
{}


BTagSFService::~BTagSFService() noexcept
{}


Service *BTagSFService::Clone() const
{
    return new BTagSFService(*this);
}


double BTagSFService::GetScaleFactor(double pt, double eta, int flavour,
  Variation var /*= Variation::Nominal*/) const
{
    // If a systematic variation is requested, make sure the service has been configured to
    //calculate it
    if (var != Variation::Nominal and not readSystematics)
        throw std::logic_error("BTagSFService::GetScaleFactor: A systematic variation is "
          "requested while the service has been configured to provide nominal scale factors "
          "only.");
    
    
    // Scale factors are not supported for jets with pt < 20 GeV
    if (pt < 20.)
        return 0.;
    
    
    // Translate jet flavour to a code
    Flavour flavourCode;
    
    switch (std::abs(flavour))
    {
        case 5:
            flavourCode = Flavour::Bottom;
            break;
        
        case 4:
            flavourCode = Flavour::Charm;
            break;
        
        default:
            flavourCode = Flavour::Light;
    }
    
    
    // Find the group of readers corresponding to this flavour
    auto const res = sfReaders.find(flavourCode);
    
    if (res == sfReaders.end())
        throw std::logic_error("BTagSFService::GetScaleFactor: Scale factor for a jet with "s +
          "flavour " + std::to_string(flavour) + " is requested, but corresponding measurement " +
          "has not been specified.");
    
    auto const &readerGroup = res->second;
    auto const &reader = readerGroup->readers[var];
    
    
    // Check if pt is outside of the range in which scale factors have been measured. If this is
    //true, clip it to the range
    bool ptOutOfRange = false;
    auto const &ptRange = reader->min_max_pt(readerGroup->translatedFlavour, eta);
    
    if (pt < ptRange.first)
    {
        ptOutOfRange = true;
        pt = ptRange.first;
    }
    else if (pt >= ptRange.second)
    {
        ptOutOfRange = true;
        pt = std::nextafter(float(ptRange.second), 0.f);
        //^ Have to use the previous representable floating-point number because
        //BTagCalibrationReader::eval performs a strict comparison with the upper bound, and it
        //uses float internally
    }
    
    
    // Calculate the scale factor
    double sf = reader->eval(readerGroup->translatedFlavour, eta, pt);
    
    
    // Double the uncertainty if pt is outside of the supported range
    if (ptOutOfRange and var != Variation::Nominal)
    {
        double const sfNominal =
          readerGroup->readers[Variation::Nominal]->eval(readerGroup->translatedFlavour, eta, pt);
        
        sf = 2 * (sf - sfNominal) + sfNominal;
    }
    
    
    return sf;
}


double BTagSFService::GetScaleFactor(Jet const &jet, Variation var /*= Variation::Nominal*/) const
{
    return GetScaleFactor(jet.Pt(), jet.Eta(), jet.Flavour(Jet::FlavourType::Hadron), var);
}


void BTagSFService::SetMeasurement(Flavour flavour, std::string const &label)
{
    // Make sure a label for this jet flavour has not been registered already
    auto const res = sfReaders.find(flavour);
    
    if (res != sfReaders.end())
        throw std::logic_error("BTagSFService::SetMeasurement: Overwriting existing "s +
          "measurement label for jet flavour " + std::to_string(unsigned(flavour)) + ".");
    
    
    // Create a group of scale factor readers for this flavour
    std::shared_ptr<ReaderSystGroup> &readerGroup = sfReaders[flavour];
    readerGroup.reset(new ReaderSystGroup);
    
    
    // Translate the jet flavour to the format used by external/BTagCalibration and store it in the
    //reader group
    BTagEntry::JetFlavor translatedFlavour;
    
    switch (flavour)
    {
        case Flavour::Bottom:
            translatedFlavour = BTagEntry::FLAV_B;
            break;
        
        case Flavour::Charm:
            translatedFlavour = BTagEntry::FLAV_C;
            break;
        
        case Flavour::Light:
            translatedFlavour = BTagEntry::FLAV_UDSG;
            break;
        
        default:
            throw std::runtime_error("BTagSFService::SetMeasurement: Unsupported jet flavour is "
              "provided.");
    }
    
    readerGroup->translatedFlavour = translatedFlavour;
    
    
    // Create and initialize individual readers
    BTagCalibrationReader *reader = new BTagCalibrationReader(translatedWP, "central");
    reader->load(*bTagCalibration.get(), translatedFlavour, label);
    readerGroup->readers[Variation::Nominal].reset(reader);
    
    if (readSystematics)
    {
        reader = new BTagCalibrationReader(translatedWP, "up");
        reader->load(*bTagCalibration.get(), translatedFlavour, label);
        readerGroup->readers[Variation::Up].reset(reader);
        
        reader = new BTagCalibrationReader(translatedWP, "down");
        reader->load(*bTagCalibration.get(), translatedFlavour, label);
        readerGroup->readers[Variation::Down].reset(reader);
    }
}


void BTagSFService::Initialize(BTagger const &bTagger, std::string const &fileName)
{
    // Translate the working point of the given tagger into a format expected by the package
    //external/BTagCalibration
    switch (bTagger.GetWorkingPoint())
    {
        case BTagger::WorkingPoint::Loose:
            translatedWP = BTagEntry::OP_LOOSE;
            break;
        
        case BTagger::WorkingPoint::Medium:
            translatedWP = BTagEntry::OP_MEDIUM;
            break;
        
        case BTagger::WorkingPoint::Tight:
            translatedWP = BTagEntry::OP_TIGHT;
            break;
        
        default:
            throw std::runtime_error("BTagSFService::Initialize: Unsupported working point of "
              "b-tagging algorithm is provided.");
    }
    
    
    // Resolve path to the CSV file with b-tagging scale factors. If the file does not exist, an
    //exception will be thrown
    std::string const filePath(FileInPath::Resolve("BTag", fileName));
    
    // Open the file
    bTagCalibration.reset(
      new BTagCalibration(BTagger::AlgorithmToTextCode(bTagger.GetAlgorithm()), filePath));
}

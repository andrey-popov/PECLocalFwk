#include <mensura/core/BTagWPService.hpp>

#include <mensura/core/FileInPath.hpp>
#include <mensura/external/JsonCpp/json.hpp>

#include <fstream>
#include <stdexcept>
#include <sstream>


BTagWPService::BTagWPService(std::string const &name, std::string const &dataFileName):
    Service(name)
{
    // If an empty file name if given, user is going to set the thresholds manually.  Thus, do not
    //attempt to read them from a file
    if (dataFileName == "")
        return;
    
    
    // Read the data JSON file
    std::string const resolvedPath(FileInPath::Resolve("BTag", dataFileName));
    std::ifstream dbFile(resolvedPath, std::ifstream::binary);
    Json::Value root;
    
    try
    {
        dbFile >> root;
    }
    catch (Json::Exception const &)
    {
        std::ostringstream message;
        message << "BTagWPService[\"" << GetName() << "\"]::BTagWPService: " <<
          "Failed to parse file \"" << resolvedPath << "\". It is not a valid JSON file, or the "
          "file is corrupted.";
        throw std::runtime_error(message.str());
    }
    
    dbFile.close();
    
    if (not root.isObject())
    {
        // The top-level entity is not a dictionary
        std::ostringstream message;
        message << "BTagWPService[\"" << GetName() << "\"]::BTagWPService: " <<
          "Top-level structure in the data file must be a dictionary. This is not true for " <<
          "file \"" << resolvedPath << "\".";
        throw std::runtime_error(message.str());
    }
    
    
    // Set thresholds
    for (auto const &t: {std::make_tuple(BTagger::Algorithm::CSV, "CSVv2"),
      std::make_tuple(BTagger::Algorithm::CMVA, "cMVAv2"),
      std::make_tuple(BTagger::Algorithm::DeepCSV, "DeepCSV")})
    {
        auto const &bTagAlgo = std::get<0>(t);
        auto const &bTagLabel = std::get<1>(t);
        
        if (not root.isMember(bTagLabel))
        {
            std::ostringstream message;
            message << "BTagWPService[\"" << GetName() << "\"]::BTagWPService: " <<
              "File \"" << resolvedPath << "\" does not contain entry for algorithm \"" <<
              bTagLabel << "\".";
            throw std::runtime_error(message.str());
        }
        
        auto const &thresholds = root[bTagLabel];
        
        try
        {
            SetThreshold({bTagAlgo, BTagger::WorkingPoint::Loose}, thresholds["L"].asDouble());
            SetThreshold({bTagAlgo, BTagger::WorkingPoint::Medium}, thresholds["M"].asDouble());
            SetThreshold({bTagAlgo, BTagger::WorkingPoint::Tight}, thresholds["T"].asDouble());
        }
        catch (Json::Exception const &)
        {
            std::ostringstream message;
            message << "BTagWPService[\"" << GetName() << "\"]::BTagWPService: " <<
              "Entry for algorithm \"" << bTagLabel << "\" in file \"" << resolvedPath <<
              "\" follows an unexpected format.";
            throw std::runtime_error(message.str());
        }
    }
}


BTagWPService::BTagWPService(std::string const &dataFile):
    BTagWPService("BTagWP", dataFile)
{}


Service *BTagWPService::Clone() const
{
    return new BTagWPService(*this);
}


double BTagWPService::GetThreshold(BTagger const &tagger) const
{
    // Find the threshold for the given working point
    auto thresholdIt = thresholds.find(tagger);
    
    if (thresholdIt == thresholds.end())
    {
        std::ostringstream ost;
        ost << "BTagWPService::IsTagged: No threshold is available for b-tagger " <<
          tagger.GetTextCode() << ".";
        
        throw std::runtime_error(ost.str());
    }
    
    
    return thresholdIt->second;
}


bool BTagWPService::IsTagged(BTagger const &tagger, Jet const &jet) const
{
    // First, check the jet pseudorapidity makes sense
    if (fabs(jet.Eta()) > BTagger::GetMaxPseudorapidity())
        // There is a very small number of tagged jets with |eta| just above 2.4
        return false;
    
    
    // Compare discriminator value to the threshold
    return (jet.BTag(tagger.GetAlgorithm()) > GetThreshold(tagger));
}


void BTagWPService::SetThreshold(BTagger const &tagger, double threshold)
{
    thresholds[tagger] = threshold;
}

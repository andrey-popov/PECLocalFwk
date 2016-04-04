#include <PECFwk/extensions/BTagWeightCSVShape.hpp>

#include <PECFwk/core/BTagger.hpp>
#include <PECFwk/core/FileInPath.hpp>
#include <PECFwk/core/JetMETReader.hpp>
#include <PECFwk/core/PhysicsObjects.hpp>
#include <PECFwk/core/Processor.hpp>
#include <PECFwk/core/ROOTLock.hpp>

#include <TFile.h>
#include <TH3.h>

#include <sstream>
#include <stdexcept>


using namespace std::literals::string_literals;


BTagWeightCSVShape::BTagWeightCSVShape(std::string const &name,
  std::string const &csvWeightFileName, double minPt_ /*= 0.*/):
    AnalysisPlugin(name),
    jetPluginName("JetMET"), jetPlugin(nullptr),
    minPt(minPt_)
{
    LoadScaleFactors(csvWeightFileName);
}


BTagWeightCSVShape::BTagWeightCSVShape(std::string const &csvWeightFileName,
  double minPt_ /*= 0.*/):
    AnalysisPlugin("BTagWeightCSVShape"),
    jetPluginName("JetMET"), jetPlugin(nullptr),
    minPt(minPt_)
{
    LoadScaleFactors(csvWeightFileName);
}


BTagWeightCSVShape::~BTagWeightCSVShape() noexcept
{}


void BTagWeightCSVShape::BeginRun(Dataset const &)
{
    // Save pointer to plugin that produces jets
    jetPlugin = dynamic_cast<JetMETReader const *>(GetDependencyPlugin(jetPluginName));
}


Plugin *BTagWeightCSVShape::Clone() const
{
    return new BTagWeightCSVShape(*this);
}


double BTagWeightCSVShape::GetWeight() const
{
    return weight;
}


void BTagWeightCSVShape::LoadScaleFactors(std::string const &csvWeightFileName)
{
    // Read histograms with scale factors
    ROOTLock::Lock();
    
    FileInPath pathBuilder;
    std::string const resolvedPath(pathBuilder.Resolve("BTag", csvWeightFileName));
    std::unique_ptr<TFile> inputFile(TFile::Open(resolvedPath.c_str()));
    
    csvScaleFactors[5].reset(dynamic_cast<TH3 *>(inputFile->Get("b")));
    csvScaleFactors[4].reset(dynamic_cast<TH3 *>(inputFile->Get("c")));
    csvScaleFactors[0].reset(dynamic_cast<TH3 *>(inputFile->Get("udsg")));
    
    ROOTLock::Unlock();
    
    
    // Make sure they all have been read. Also detach them from the file, so that they are not
    //deleted when the file is closed.
    for (auto &p: csvScaleFactors)
    {
        if (not p.second)
        {
            std::ostringstream ost;
            ost << "BTagWeightCSVShape::LoadScaleFactors: Failed to find histogram for flavour " <<
              p.first << " in file \"" << resolvedPath << "\".";
            throw std::runtime_error(ost.str());
        }
        
        p.second->SetDirectory(nullptr);
    }
}


bool BTagWeightCSVShape::ProcessEvent()
{
    weight = 1.;
    
    
    // Loop over jets in the current event
    for (auto const &jet: jetPlugin->GetJets())
    {
        // Skip jets that fail the pt cut or fall outside of the tracker acceptance
        if (jet.Pt() < minPt or std::fabs(jet.Eta()) > BTagger::GetMaxPseudorapidity())
            continue;
        
        
        // Find the histogram corresponding to the flavour of the current jet
        unsigned flavour = std::abs(jet.GetParentID());
        
        if (flavour == 21 or flavour < 4)
            flavour = 0;
        
        auto const histIt = csvScaleFactors.find(flavour);
        
        if (histIt == csvScaleFactors.end())
            throw std::runtime_error("BTagWeightCSVShape::ProcessEvent: Cannot find scale "s +
              "factors for jet flavour " + std::to_string(jet.GetParentID()) + ".");
        
        
        // Update the event weight
        auto const &h = histIt->second;
        double const sf = h->GetBinContent(h->FindFixBin(jet.Pt(), std::fabs(jet.Eta()),
          jet.BTag(BTagger::Algorithm::CSV)));
        
        if (sf != 0.)
            weight *= sf;
    }
    
    
    // This plugin does not perform event filtering, so always return true
    return true;
}

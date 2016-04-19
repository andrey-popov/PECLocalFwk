#include <mensura/extensions/BTagEffHistograms.hpp>

#include <mensura/core/BTagWPService.hpp>
#include <mensura/core/JetMETReader.hpp>
#include <mensura/core/Processor.hpp>
#include <mensura/extensions/PileUpWeight.hpp>
#include <mensura/extensions/TFileService.hpp>

#include <cmath>


using namespace std::literals::string_literals;


BTagEffHistograms::BTagEffHistograms(std::string const &name, BTagger::Algorithm algo_,
  std::list<BTagger::WorkingPoint> const &workingPoints_):
    AnalysisPlugin(name),
    algo(algo_), workingPoints(workingPoints_),
    fileServiceName("TFileService"), fileService(nullptr),
    jetPluginName("JetMET"), jetPlugin(nullptr),
    bTagWPServiceName("BTagWP"), bTagWPService(nullptr),
    puWeightPluginName("PileUpWeight"), puWeightPlugin(nullptr)
{
    Initialize();
}


BTagEffHistograms::BTagEffHistograms(BTagger::Algorithm algo_,
  std::list<BTagger::WorkingPoint> const &workingPoints_):
    AnalysisPlugin("BTagEffHistograms"),
    algo(algo_), workingPoints(workingPoints_),
    fileServiceName("TFileService"), fileService(nullptr),
    jetPluginName("JetMET"), jetPlugin(nullptr),
    bTagWPServiceName("BTagWP"), bTagWPService(nullptr),
    puWeightPluginName("PileUpWeight"), puWeightPlugin(nullptr)
{
    Initialize();
}


BTagEffHistograms::~BTagEffHistograms() noexcept
{}


void BTagEffHistograms::BeginRun(Dataset const &)
{
    // Save pointers to other plugins and services
    fileService = dynamic_cast<TFileService const *>(GetMaster().GetService(fileServiceName));
    bTagWPService = dynamic_cast<BTagWPService const *>(GetMaster().GetService(bTagWPServiceName));
    
    jetPlugin = dynamic_cast<JetMETReader const *>(GetDependencyPlugin(jetPluginName));
    puWeightPlugin = dynamic_cast<PileUpWeight const *>(GetDependencyPlugin(puWeightPluginName));
    
    
    // Construct the histograms for all jet flavours
    std::string const algoLabel(BTagger::AlgorithmToTextCode(algo));
    
    for (auto const &f: {std::make_pair("b"s, 5), std::make_pair("c"s, 4),
      std::make_pair("udsg"s, 0)})
    {
        HistFlavourGroup &group = histGroups[f.second];
        
        group.denominator =
          fileService->Create<TH2D>(algoLabel.c_str(), (f.first + "_All").c_str(),
          "All jets;p_{T};|#eta|",
          ptBinning.size() - 1, ptBinning.data(), etaBinning.size() - 1, etaBinning.data());
        
        for (auto const &wp: workingPoints)
            group.numerator[wp] =
              fileService->Create<TH2D>(algoLabel.c_str(),
                (f.first + "_" + BTagger::WorkingPointToTextCode(wp)).c_str(),
                "Jets passing given working point;p_{T};|#eta|",
                ptBinning.size() - 1, ptBinning.data(), etaBinning.size() - 1, etaBinning.data());
    }
}


Plugin *BTagEffHistograms::Clone() const
{
    return new BTagEffHistograms(*this);
}


void BTagEffHistograms::EndRun()
{
    // Clear the map with histograms. The histograms will be deleted when the output file is closed
    histGroups.clear();
}


void BTagEffHistograms::SetEtaBinning(std::vector<double> const &etaBinning_)
{
    etaBinning.clear();
    etaBinning.reserve(etaBinning_.size());
    
    for (auto const &edge: etaBinning_)
        etaBinning.push_back(edge);
}


void BTagEffHistograms::SetPtBinning(std::vector<double> const &ptBinning_)
{
    ptBinning.clear();
    ptBinning.reserve(ptBinning_.size());
    
    for (auto const &edge: ptBinning_)
        ptBinning.push_back(edge);
}


void BTagEffHistograms::Initialize()
{
    // Set default binning in pt and |eta|
    for (double pt = 20.; pt < 99.; pt += 5.)
        ptBinning.push_back(pt);
    
    for (double pt = 100.; pt < 1001.; pt += 10.)
        ptBinning.push_back(pt);
    
    etaBinning = {0., 0.6, 1.2, 1.8, 2.4};
}


bool BTagEffHistograms::ProcessEvent()
{
    // Calculate event weight
    double const weight = puWeightPlugin->GetWeight();
    
    
    // Loop over reconstructed jets
    for (auto const &j: jetPlugin->GetJets())
    {
        // Determine jet flavour. All light-flavour jets are considered together
        unsigned flavour = std::abs(j.GetParentID());
        
        if (flavour != 4 and flavour != 5)
            flavour = 0;
        
        
        auto const &group = histGroups.at(flavour);
        
        
        // Fill the histograms
        double const pt = j.Pt();
        double const eta = std::fabs(j.Eta());
        
        group.denominator->Fill(pt, eta, weight);
        
        for (auto const &wp: workingPoints)
        {
            if (bTagWPService->IsTagged(BTagger(algo, wp), j))
                group.numerator.at(wp)->Fill(pt, eta, weight);
        }
    }
    
    
    // Since this plugin does not perform event filtering, always return true
    return true;
}

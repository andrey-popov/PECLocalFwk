#include <PECFwk/extensions/MetFilter.hpp>

#include <PECFwk/core/LeptonReader.hpp>
#include <PECFwk/core/JetMETReader.hpp>
#include <PECFwk/core/Processor.hpp>

#include <cmath>


using namespace std;


MetFilter::MetFilter(std::string const &name, Mode mode_, double threshold_):
    AnalysisPlugin(name),
    mode(mode_), threshold(threshold_),
    metPluginName("JetMET"), metPlugin(nullptr),
    leptonPluginName("Leptons"), leptonPlugin(nullptr)
{}


MetFilter::MetFilter(Mode mode_, double threshold_):
    AnalysisPlugin("MetFilter"),
    mode(mode_), threshold(threshold_),
    metPluginName("JetMET"), metPlugin(nullptr),
    leptonPluginName("Leptons"), leptonPlugin(nullptr)
{}


MetFilter::MetFilter(double threshold_):
    AnalysisPlugin("MetFilter"),
    mode(Mode::MET), threshold(threshold_),
    metPluginName("JetMET"), metPlugin(nullptr),
    leptonPluginName("Leptons"), leptonPlugin(nullptr)
{}


MetFilter::~MetFilter()
{}


void MetFilter::BeginRun(Dataset const &)
{
    // Save pointer to plugin that produces MET
    metPlugin = dynamic_cast<JetMETReader const *>(
      GetMaster().GetPluginBefore(metPluginName, GetName()));
    
    
    // In case of filtering on MtW, also save pointer to plugin producing leptons
    if (mode == Mode::MtW)
        leptonPlugin = dynamic_cast<LeptonReader const *>(
          GetMaster().GetPluginBefore(leptonPluginName, GetName()));
}


Plugin *MetFilter::Clone() const
{
    return new MetFilter(*this);
}


bool MetFilter::ProcessEvent()
{
    MET const &met = metPlugin->GetMET();
    
    if (mode == Mode::MET)
        return (met.Pt() > threshold);
    else
    {
        auto const &leptons = leptonPlugin->GetLeptons();
        
        // Reject event if there are no leptons
        if (leptons.size() == 0)
            return false;
        
        
        // Calculate MtW and apply the selection
        auto const &l = leptons.front();
        double const MtW = std::sqrt(std::pow(l.Pt() + met.Pt(), 2) -
          std::pow(l.P4().Px() + met.P4().Px(), 2) - std::pow(l.P4().Py() + met.P4().Py(), 2));
        
        return (MtW > threshold);
    }
}

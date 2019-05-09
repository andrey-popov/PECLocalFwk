#include <mensura/LeptonFilter.hpp>

#include <mensura/LeptonReader.hpp>
#include <mensura/Processor.hpp>


LeptonFilter::SelectionBin::SelectionBin(Lepton::Flavour flavour_, double minPt_,
  double maxAbsEta_) noexcept:
    flavour(flavour_), minPt(minPt_), maxAbsEta(maxAbsEta_),
    counts(0)
{}


void LeptonFilter::SelectionBin::Clear()
{
    counts = 0;
}


bool LeptonFilter::SelectionBin::Fill(Lepton const &lepton)
{
    if (lepton.GetFlavour() != flavour or lepton.Pt() <= minPt)
        return false;
    
    if (flavour == Lepton::Flavour::Electron)
    {
        if (fabs(lepton.UserFloat("etaSC")) >= maxAbsEta)
            return false;
    }
    else
    {
        if (fabs(lepton.Eta()) >= maxAbsEta)
            return false;
    }
    
    
    // The lepton passes the selection if the workflow reaches this point
    ++counts;
    return true;
}



LeptonFilter::LeptonFilter(std::string const name /*= "LeptonFilter"*/) noexcept:
    AnalysisPlugin(name),
    leptonPluginName("Leptons")
{}


LeptonFilter::LeptonFilter(std::string const &name, Lepton::Flavour flavour, double minPt,
  double maxAbsEta, unsigned duplicates /*= 1*/) noexcept:
    AnalysisPlugin(name),
    leptonPluginName("Leptons")
{
    AddSelectionBin(flavour, minPt, maxAbsEta, duplicates);
}


LeptonFilter::~LeptonFilter() noexcept
{}


void LeptonFilter::AddSelectionBin(Lepton::Flavour flavour, double minPt, double maxAbsEta,
  unsigned duplicates /*= 1*/)
{
    for (unsigned i = 0; i < duplicates; ++i)
        bins.emplace_back(flavour, minPt, maxAbsEta);
}


void LeptonFilter::BeginRun(Dataset const &)
{
    // Save pointer to plugin that produces leptons
    leptonPlugin = dynamic_cast<LeptonReader const *>(GetDependencyPlugin(leptonPluginName));
}


Plugin *LeptonFilter::Clone() const
{
    return new LeptonFilter(*this);
}


bool LeptonFilter::ProcessEvent()
{
    // Clear counters from the previous event
    for (auto &b: bins)
        b.Clear();
    
    
    // Count how many tight leptons fall into each selection bin and how many tight leptons fall
    //into at least one bin
    unsigned nLeptonsSelected = 0;
    
    for (auto const &l: leptonPlugin->GetLeptons())
    {
        bool isSelected = false;
        
        for (auto &b: bins)
        {
            if (b.Fill(l))
                isSelected = true;
        }
        
        if (isSelected)
            ++nLeptonsSelected;
    }
    
    
    // If at least one selection bin remains empty, reject the event
    for (auto const &b: bins)
        if (b.counts == 0)
            return false;
    
    
    // If the number of selected leptons is not what is demanded by the selection (here it can only
    //be larger), reject the event
    if (nLeptonsSelected != bins.size())
        return false;
    
    
    // If, in addition to the selected tight leptons, the event contains some loose leptons, reject
    //the event. This condition exploits the fact that the collection of loose leptons includes all
    //tight leptons as well
    if (leptonPlugin->GetLooseLeptons().size() != nLeptonsSelected)
        return false;
    
    
    // If control flow has reached this point, the event passes the selection
    return true;
}

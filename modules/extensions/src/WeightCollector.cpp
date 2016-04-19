#include <mensura/extensions/WeightCollector.hpp>

#include <sstream>
#include <stdexcept>


WeightCollector::WeightCollector(std::string const &name,
  std::initializer_list<std::string> const &weightPluginNames /*= {}*/) noexcept:
    AnalysisPlugin(name)
{
    for (auto const &name: weightPluginNames)
        weightPlugins[name] = nullptr;
}


WeightCollector::WeightCollector(std::initializer_list<std::string> const &weightPluginNames)
  noexcept:
    AnalysisPlugin("EventWeights")
{
    for (auto const &name: weightPluginNames)
        weightPlugins[name] = nullptr;
}


void WeightCollector::AddWeightPlugin(std::string const &name)
{
    weightPlugins[name] = nullptr;
}


void WeightCollector::BeginRun(Dataset const &)
{
    for (auto &p: weightPlugins)
    {
        p.second = dynamic_cast<EventWeightPlugin const *>(GetDependencyPlugin(p.first));
        
        // Make sure that dynamic_cast succeeded
        if (not p.second)
        {
            std::ostringstream message;
            message << "WeightCollector[\"" << GetName() << "\"]::BeginRun: Plugin \"" <<
              p.first << "\" cannot be cast to type EventWeightPlugin.";
            throw std::runtime_error(message.str());
        }
    }
}


Plugin *WeightCollector::Clone() const
{
    return new WeightCollector(*this);
}


EventWeightPlugin const *WeightCollector::GetPlugin(std::string const &name) const
{
    auto const res = weightPlugins.find(name);
    
    if (res == weightPlugins.end())
    {
        std::ostringstream message;
        message << "WeightCollector[\"" << GetName() << "\"]::GetPlugin: Plugin \"" <<
          name << "\" is not known to this collector.";
        throw std::runtime_error(message.str());
    }
    
    return res->second;
}


double WeightCollector::GetWeight() const
{
    double weight = 1.;
    
    for (auto const &p: weightPlugins)
        weight *= p.second->GetWeight();
    
    return weight;
}


double WeightCollector::GetWeightDown(std::string const &pluginName, unsigned iVar) const
{
    EventWeightPlugin const *pluginShiftedWeight = GetPlugin(pluginName);
    double weight = 1.;
    
    for (auto const &p: weightPlugins)
    {
        if (p.second == pluginShiftedWeight)
            weight *= p.second->GetWeightDown(iVar);
        else
            weight *= p.second->GetWeight();
    }
    
    return weight;
}


double WeightCollector::GetWeightUp(std::string const &pluginName, unsigned iVar) const
{
    EventWeightPlugin const *pluginShiftedWeight = GetPlugin(pluginName);
    double weight = 1.;
    
    for (auto const &p: weightPlugins)
    {
        if (p.second == pluginShiftedWeight)
            weight *= p.second->GetWeightUp(iVar);
        else
            weight *= p.second->GetWeight();
    }
    
    return weight;
}


bool WeightCollector::ProcessEvent()
{
    return true;
}

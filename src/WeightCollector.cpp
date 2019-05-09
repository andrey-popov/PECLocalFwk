#include <mensura/WeightCollector.hpp>

#include <sstream>
#include <stdexcept>


WeightCollector::WeightCollector(std::string const &name,
  std::initializer_list<std::string> const &weightPluginNames /*= {}*/) noexcept:
    AnalysisPlugin(name),
    weightPlugins(weightPluginNames.size(), nullptr)
{
    unsigned pluginIndex = 0;
    
    for (auto const &name: weightPluginNames)
    {
        weightPluginIndices[name] = pluginIndex;
        ++pluginIndex;
    }
}


WeightCollector::WeightCollector(std::initializer_list<std::string> const &weightPluginNames)
  noexcept:
    AnalysisPlugin("EventWeights"),
    weightPlugins(weightPluginNames.size(), nullptr)
{
    unsigned pluginIndex = 0;
    
    for (auto const &name: weightPluginNames)
    {
        weightPluginIndices[name] = pluginIndex;
        ++pluginIndex;
    }
}


void WeightCollector::AddWeightPlugin(std::string const &name)
{
    weightPlugins.emplace_back(nullptr);
    weightPluginIndices[name] = weightPlugins.size() - 1;
}


void WeightCollector::BeginRun(Dataset const &)
{
    for (auto &p: weightPluginIndices)
    {
        auto const &pluginName = p.first;
        auto const &pluginIndex = p.second;
        
        auto const *plugin =
          dynamic_cast<EventWeightPlugin const *>(GetDependencyPlugin(pluginName));
        
        // Make sure that dynamic_cast succeeded
        if (not plugin)
        {
            std::ostringstream message;
            message << "WeightCollector[\"" << GetName() << "\"]::BeginRun: Plugin \"" <<
              pluginName << "\" cannot be cast to type EventWeightPlugin.";
            throw std::runtime_error(message.str());
        }
        
        weightPlugins[pluginIndex] = plugin;
    }
}


Plugin *WeightCollector::Clone() const
{
    return new WeightCollector(*this);
}


unsigned WeightCollector::GetNumPlugins() const
{
    return weightPlugins.size();
}


EventWeightPlugin const *WeightCollector::GetPlugin(std::string const &name) const
{
    auto const res = weightPluginIndices.find(name);
    
    if (res == weightPluginIndices.end())
    {
        std::ostringstream message;
        message << "WeightCollector[\"" << GetName() << "\"]::GetPlugin: Plugin \"" <<
          name << "\" is not known to this collector.";
        throw std::runtime_error(message.str());
    }
    
    return weightPlugins[res->second];
}


EventWeightPlugin const *WeightCollector::GetPlugin(unsigned index) const
{
    if (index >= weightPlugins.size())
    {
        std::ostringstream message;
        message << "WeightCollector[\"" << GetName() << "\"]::GetPlugin: Plugin index " <<
          index << " is out of range.";
        throw std::runtime_error(message.str());
    }
    
    return weightPlugins[index];
}


double WeightCollector::GetWeight() const
{
    double weight = 1.;
    
    for (auto const &p: weightPlugins)
        weight *= p->GetWeight();
    
    return weight;
}


double WeightCollector::GetWeightDown(std::string const &pluginName, unsigned iVar) const
{
    auto const res = weightPluginIndices.find(pluginName);
    
    if (res == weightPluginIndices.end())
    {
        std::ostringstream message;
        message << "WeightCollector[\"" << GetName() << "\"]::GetWeightDown: Plugin \"" <<
          pluginName << "\" is not known to this collector.";
        throw std::runtime_error(message.str());
    }
    
    return GetWeightDown(res->second, iVar);
}


double WeightCollector::GetWeightDown(unsigned index, unsigned iVar) const
{
    EventWeightPlugin const *pluginShiftedWeight = GetPlugin(index);
    double weight = 1.;
    
    for (auto const &p: weightPlugins)
    {
        if (p == pluginShiftedWeight)
            weight *= p->GetWeightDown(iVar);
        else
            weight *= p->GetWeight();
    }
    
    return weight;
}


double WeightCollector::GetWeightUp(std::string const &pluginName, unsigned iVar) const
{
    auto const res = weightPluginIndices.find(pluginName);
    
    if (res == weightPluginIndices.end())
    {
        std::ostringstream message;
        message << "WeightCollector[\"" << GetName() << "\"]::GetWeightUp: Plugin \"" <<
          pluginName << "\" is not known to this collector.";
        throw std::runtime_error(message.str());
    }
    
    return GetWeightUp(res->second, iVar);
}


double WeightCollector::GetWeightUp(unsigned index, unsigned iVar) const
{
    EventWeightPlugin const *pluginShiftedWeight = GetPlugin(index);
    double weight = 1.;
    
    for (auto const &p: weightPlugins)
    {
        if (p == pluginShiftedWeight)
            weight *= p->GetWeightUp(iVar);
        else
            weight *= p->GetWeight();
    }
    
    return weight;
}


bool WeightCollector::ProcessEvent()
{
    return true;
}

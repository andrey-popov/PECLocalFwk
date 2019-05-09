#pragma once

#include <mensura/AnalysisPlugin.hpp>

#include <mensura/EventWeightPlugin.hpp>

#include <initializer_list>
#include <list>
#include <map>
#include <vector>


/**
 * \class WeightCollector
 * \brief A plugin that combines weights from several instances of EventWeightPlugin
 * 
 * This plugin simplifies handling of event reweighting with multiple plugins. It computes a
 * product of weights calculated by an arbitrary number of associated reweighting plugins. It also
 * allows to access systematic variations of weights.
 */
class WeightCollector: public AnalysisPlugin
{
public:
    /**
     * \brief Creates a new instance with the given name
     * 
     * The optional argument provides names of reweighting plugins to be associated with this
     * collector.
     */
    WeightCollector(std::string const &name,
      std::initializer_list<std::string> const &weightPluginNames = {}) noexcept;
    
    /// A short-cut for the above version with a default name "EventWeights"
    WeightCollector(std::initializer_list<std::string> const &weightPluginNames) noexcept;
    
public:
    /// Adds a new reweighting plugin to the list of associated ones
    void AddWeightPlugin(std::string const &name);
    
    /**
     * \brief Saves pointers to all associated reweighting plugins
     * 
     * Reimplemented from Plugin.
     */
    virtual void BeginRun(Dataset const &) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Plugin.
     */
    virtual Plugin *Clone() const override;
    
    /// Returns the number of registered reweighting plugins
    unsigned GetNumPlugins() const;
    
    /**
     * \brief Returns a non-owning pointer to one of the associated reweighting plugins
     * 
     * Throws an exception if the plugin with the given name is not found among the ones associated
     * with this collector.
     */
    EventWeightPlugin const *GetPlugin(std::string const &name) const;
    
    /// Alternative version that accesses reweighting plugin by index
    EventWeightPlugin const *GetPlugin(unsigned index) const;
    
    /**
     * \brief Computes the nominal weight for the current event
     * 
     * Calculates as the product of nominal weights given by every associated reweighting plugin.
     */
    double GetWeight() const;
    
    /**
     * \brief Computes full event weight with a down systematic variation in the given reweighting
     * plugin
     * 
     * From the reweighting plugin with the given name the down variation corresponding to the
     * given index is requested. It is multiplied by nominal weights for all other plugins. Throws
     * an exception if the plugin with the given name is not associated with this collector.
     */
    double GetWeightDown(std::string const &pluginName, unsigned iVar) const;
    
    /// Alternative version that accesses reweighting plugin by index
    double GetWeightDown(unsigned index, unsigned iVar) const;
    
    /**
     * \brief Computes full event weight with an up systematic variation in the given reweighting
     * plugin
     * 
     * Consult documentation for method GetWeightDown for details.
     */
    double GetWeightUp(std::string const &pluginName, unsigned iVar) const;
    
    /// Alternative version that accesses reweighting plugin by index
    double GetWeightUp(unsigned index, unsigned iVar) const;
    
private:
    /**
     * \brief Does nothing
     * 
     * Implemented from Plugin.
     */
    virtual bool ProcessEvent() override;
    
private:
    /// Associated reweighting plugins
    std::vector<EventWeightPlugin const *> weightPlugins;
    
    /// Map into indices of associated reweighting plugins
    std::map<std::string, unsigned> weightPluginIndices;
};

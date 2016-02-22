#pragma once

#include <PECFwk/core/AnalysisPlugin.hpp>

#include <PECFwk/core/PhysicsObjects.hpp>


class JetMETReader;


/**
 * \class MetFilter
 * \brief Performs additional selection on value of MET
 * 
 * The plugin checks that the value of MET in an event is larger than the given threshold.
 */
class MetFilter: public AnalysisPlugin
{
public:
    /**
     * \brief Constructor
     */
    MetFilter(double threshold);
    
public:
    /**
     * \brief Constructs a newly-initialised copy
     * 
     * Consult documentation of the overriden method for details.
     */
    Plugin *Clone() const;
    
    /**
     * \brief Notifies this that a dataset has been opened
     * 
     * Only updates the pointer to the reader plugin.
     */
    void BeginRun(Dataset const &);
    
private:
    /// Peforms filtering
    bool ProcessEvent();
    
private:
    /// Name of the plugin that produces MET
    std::string metPluginName;
    
    /// Non-owning pointer to a plugin that produces MET
    JetMETReader const *metPlugin;
    
    /// Threshold on MET
    double threshold;
};

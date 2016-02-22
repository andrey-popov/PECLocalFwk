#pragma once

#include <PECFwk/core/AnalysisPlugin.hpp>

#include <PECFwk/core/PhysicsObjects.hpp>


class JetMETReader;
class LeptonReader;


/**
 * \class MetFilter
 * \brief Filter on MET or MtW
 * 
 * Depending on configuration, the plugin selects events in which value of MET or MtW is larger
 * that the given threshold. The filter relies on the presence of a JetMETReader and (only for MtW)
 * a LeptonReader with default names "JetMET" and "Leptons". The value of MtW is evaluated using
 * the leading tight lepton; if the event does not contain any lepton, it is rejected.
 */
class MetFilter: public AnalysisPlugin
{
public:
    /// Supported filtering modes
    enum class Mode
    {
        MET,
        MtW
    };
    
public:
    /// Constructor
    MetFilter(std::string const &name, Mode mode, double threshold);
    
    /**
     * \brief Constructor
     * 
     * A short-cut for the above version with the default plugin name ("MetFilter").
     */
    MetFilter(Mode mode, double threshold);
    
    /**
     * \brief Constructor
     * 
     * A short-cut with a default name ("MetFilter") and selection on MET.
     */
    MetFilter(double threshold);
    
    /// Default copy constructor
    MetFilter(MetFilter const &) = default;
    
    /// Default move constructor
    MetFilter(MetFilter &&) = default;
    
    /// Assignment operator is deleted
    MetFilter &operator=(MetFilter const &) = delete;
    
    /// Trivial destructor
    virtual ~MetFilter();
    
public:
    /**
     * \brief Notifies this that a dataset has been opened
     * 
     * Only updates the pointer to the reader plugin.
     */
    void BeginRun(Dataset const &);
    
    /**
     * \brief Constructs a newly-initialised copy
     * 
     * Consult documentation of the overriden method for details.
     */
    Plugin *Clone() const;
    
private:
    /**
     * \brief Calculates MET or MtW and applies the selection
     * 
     * Implemented from Plugin.
     */
    bool ProcessEvent();
    
private:
    /// Selected mode of filtering
    Mode mode;
    
    /// Threshold on MET or MtW, GeV
    double threshold;
    
    /// Name of the plugin that produces MET
    std::string metPluginName;
    
    /// Non-owning pointer to a plugin that produces MET
    JetMETReader const *metPlugin;
    
    /**
     * \brief Name of the plugin that produces leptons
     * 
     * Set to an empty string ("") when filtering on MET
     */
    std::string leptonPluginName;
    
    /**
     * \brief Non-owning pointer to a plugin that produces leptons
     * 
     * Set to nullptr when filtering on MET
     */
    LeptonReader const *leptonPlugin;
};

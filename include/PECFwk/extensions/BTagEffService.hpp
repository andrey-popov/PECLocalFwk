#pragma once

#include <PECFwk/core/Service.hpp>

#include <PECFwk/core/BTagger.hpp>
#include <PECFwk/core/Dataset.hpp>

#include <TFile.h>
#include <TH2D.h>

#include <list>
#include <map>
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>


class Jet;


/**
 * \class BTagEffService
 * \brief Service to accesses b-tagging efficiencies stored in a ROOT file
 * 
 * The class implements access to b-tagging efficiencies in a generic way. The efficiencies are
 * stored in a ROOT file in the form of 2D histograms in jet transverse momentum and
 * pseudorapidity. Supported format of the file is described below.
 * 
 * Histograms are organized in directories named after b tagger codes (as returned by
 * BTagger::GetTextCode). The are named following the pattern processLabel_flavour. The process
 * label is an arbitrary test label that helps to distinguish between histograms for different
 * processes. The correspondence between datasets and process labels is defined with the help of
 * methods SetProcessLabel and SetDefaultProcessLabel. The last part of the histogram name, the
 * flavour label, takes one of the following values: "b", "c", "udsg".
 * 
 * Directories with efficiency histograms can be be placed in other directories in the input file.
 * This allows to store histograms for several versions of event selection, for instance.
 */
class BTagEffService: public Service
{
public:
    /**
     * \brief Creates a service with the given name
     * 
     * The path to the ROOT file containing b-tagging efficiencies may include names of in-file
     * directories separated from the rest of the path with a colon. The path is resolved with the
     * help of FileInPath class adding a postfix "BTag/" to the standard location. If the file is
     * not found or the provided path is misformatted, an exception is thrown.
     */
    BTagEffService(std::string const &name, std::string const &path);
    
    /// A short-cut for the above version with a default name "BTagEff"
    BTagEffService(std::string const &path);
    
    /// Default move constructor
    BTagEffService(BTagEffService &&) = default;
    
    /// Assignment operator is deleted
    BTagEffService &operator=(BTagEffService const &) = delete;
    
    /// Trivial virtual destructor
    virtual ~BTagEffService() noexcept;
    
private:
    /**
     * \brief Copy constructor
     * 
     * The input file is shared with the source. The map with efficiency histograms is not copied.
     */
    BTagEffService(BTagEffService const &src) noexcept;
    
public:
    /**
     * \brief Updates label of the current process and resets the map with efficiency histograms if
     * needed
     * 
     * Reimplemented from Service.
     */
    virtual void BeginRun(Dataset const &dataset) override;
    
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Service.
     */
    virtual Service *Clone() const override;
    
    /**
     * \brief Returns b-tagging efficiency for the given b tagger and given jet properties
     * 
     * Loads histograms for the given b tagger if needed. If the histogram with efficiencies is not
     * found, an exception is thrown.
     */
    double GetEfficiency(BTagger const &bTagger, double pt, double eta, unsigned flavour) const;
    
    /// Short-cut for the overloaded version above
    double GetEfficiency(BTagger const &bTagger, Jet const &jet) const;
    
    /// Sets the default label to be used when no label is found for a process code
    void SetDefaultProcessLabel(std::string const &label);
    
    /**
     * \brief Specifies a label to which the given process code should be mapped
     * 
     * The label is later used to construct the name of the histogram with efficiencies as
     * described in the documentation of the class. The method can be called several times to
     * specify mapping of different process codes. The mapping rules are stored in the order of
     * their specification. If the method is called for a process code that has already been
     * mapped, the corresponding label is updated.
     */
    void SetProcessLabel(Dataset::Process code, std::string const &label);
    
    /**
     * \brief Sequentially calls the overloaded version with a single process code for all provided
     * values, in the order of their appearance.
     */
    void SetProcessLabel(std::list<Dataset::Process> const &codes, std::string const &label);
    
private:
    /// Reads histograms with efficiencies for the given b tagger and current process label
    void LoadEfficiencies(BTagger const &bTagger);
    
    /// Opens input file and extracts name of the in-file directory
    void OpenInputFile(std::string const &path);
    
private:
    /**
     * \brief Input ROOT file with b-tagging efficiencies
     * 
     * The file is shared among all clones of this.
     */
    std::shared_ptr<TFile> srcFile;
    
    /// Directory in the input ROOT file that contains histograms with b-tagging efficiencies
    std::string inFileDirectory;
    
    /**
     * \brief Correspondance between process codes and labels of efficiency histograms
     * 
     * The labels are used to build names of histograms with efficiencies. The container is sorted
     * in the order in which process codes were registered with SetProcessLabel (and for this
     * reason it cannot be implemented as an std::map). Process codes must not repeat.
     */
    std::list<std::pair<Dataset::Process, std::string>> processLabelMap;
    
    /// Label to be used with process codes for which no mapping rule is defined in processMap
    std::string defaultProcessLabel;
    
    /// Process label for the current dataset
    std::string curProcessLabel;
    
    /**
     * \brief Map with b-tagging efficiency histograms
     * 
     * The key of the enclosed map is the absolute value of jet flavour.
     */
    mutable std::unordered_map<BTagger, std::map<unsigned, std::shared_ptr<TH2>>> effHists;
};

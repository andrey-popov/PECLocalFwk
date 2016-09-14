#pragma once

#include <mensura/core/Service.hpp>

#include <mensura/core/BTagger.hpp>
#include <mensura/core/Dataset.hpp>

#include <TFile.h>
#include <TH2D.h>

#include <list>
#include <map>
#include <memory>
#include <regex>
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>


class Jet;


/**
 * \class BTagEffService
 * \brief Service to accesses b-tagging efficiencies stored in a ROOT file
 * 
 * This class implements access to b-tagging efficiencies in a generic way. The efficiencies are
 * stored in a ROOT file in the form of 2D histograms in jet transverse momentum and
 * pseudorapidity. Supported format of the file is described below.
 * 
 * Histograms are organized in directories named after b tagger codes (as returned by
 * BTagger::GetTextCode). They are named following the pattern {label}_{flavour}. The efficiency
 * label is an arbitrary string that allows to distinguish between different physics processes or
 * datasets. The correspondence between dataset ID, as returned by Dataset::GetSourceDatasetID,
 * is defined using methods SetEffLabel and SetDefaultEffLabel. The last part of the histogram name
 * is the jet flavour label, which can take values "b", "c", or "udsg".
 * 
 * Histograms with efficiencies may be placed in in-file directories. This is useful to store
 * efficiencies for multiple versions of event selection.
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
    
private:
    /**
     * \brief Copy constructor
     * 
     * The input file is shared with the source. The map with efficiency histograms is not copied.
     */
    BTagEffService(BTagEffService const &src) noexcept;
    
public:
    /**
     * \brief Updates efficiency label for the new dataset and resets the map with efficiency
     * histograms if needed
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
    
    /**
     * \brief Specifies an efficiency label to be used with datasets whose ID match the given mask
     * 
     * The efficiency label is used to identify histograms in the input file as described in the
     * class documentation. The mask must be a valid regular expression. Each call to this method
     * creates a new rule that matches an efficiency label to a mask.
     * 
     * When a new dataset is opened, its source dataset ID will be matched to masks in the rules in
     * the order of their specification. The first match found will provide the label to be used
     * with the dataset.
     */
    void SetEffLabel(std::string const &datasetIdMask, std::string const &label);
    
    /// Repeatedly calls the overloaded version for each (mask, label) pair
    void SetEffLabel(std::initializer_list<std::pair<std::string, std::string>> const &rules);
    
    /// Sets the default label to be used when the current dataset does not match any rules
    void SetDefaultEffLabel(std::string const &label);
    
private:
    /// Reads histograms with efficiencies for the given b tagger and current efficiency label
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
     * \brief Correspondence between masks for source dataset ID and efficiency label
     * 
     * The rules are stored in the order of their definition by method SetEffLabel.
     */
    std::vector<std::pair<std::regex, std::string>> effLabelRules;
    
    /// Efficiency label to be used for datasets that do not match any rules
    std::string defaultEffLabel;
    
    /// Efficiency label for the current dataset
    std::string curEffLabel;
    
    /**
     * \brief Map with b-tagging efficiency histograms
     * 
     * The key of the enclosed map is the absolute value of jet flavour.
     */
    mutable std::unordered_map<BTagger, std::map<unsigned, std::shared_ptr<TH2>>> effHists;
};

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
#include <utility>


class Jet;


/**
 * \class BTagEffService
 * \brief Service to accesses b-tagging efficiencies stored in a ROOT file
 * 
 * The class implements an access to b-tagging efficiencies in a generic way. The efficiencies are
 * stored in a ROOT file in the form of 2D histograms in jet transverse momentum and pseudorapidity.
 * Supported format of the file is described below.
 * 
 * The histograms are named following the pattern processLabel_flavour_workingPoint. Here
 * processLabel is an arbitrary label that helps to distinguish between histograms for different
 * processes. The label must not contain an underscore, which is reserved to separate parts of the
 * histogram name. The next fragment, flavour, takes on one of the following values: "b", "c",
 * "uds", "g"; jets with unidentified flavour (zero) should be assigned to the gluon category. The
 * last part denotes the working point of the b-tagging algorithm; its possible values are "T", "M",
 * and "L", as specified in method WorkingPointToText. The file format does not require that
 * histograms for all combinations of working points and jet flavours are present in the file.
 * 
 * The file can contain several alternative sets of histograms, for example, to describe different
 * versions of event selection. Each set must be placed in a dedicated in-file directory. The
 * directories can have arbitrary names.
 * 
 * The root directory of the file must contain a TObjString object with name "formatVersion". Its
 * title value encodes version of file format. The following values are supported at the moment:
 * "1.0".
 * 
 * The user creates an instance of the class providing a path to a ROOT file with b-tagging
 * efficiencies and an in-file path to a directory that contains the desired set of histograms. Then
 * the user must define with the help of method SetProcesslabel a desired mapping between process
 * codes assigned to datasets and process labels used to form histogram names. (S)he can also
 * specify a default process label to be used when no specific label is provided.
 * 
 * After this is done, b-tagging efficiencies for a given dataset can be loaded read with the method
 * LoadPayload and then accessed using GetEfficiency.
 * 
 * The class provides valid copy and move constructors. It is not thread-safe; however, the ROOT
 * file and histograms with b-tagging efficiences are safe be shared among several threads.
 */
class BTagEffService: public Service
{
public:
    /**
     * \brief Creates a service with the given name
     * 
     * The arguments are the path to a ROOT file with b-tagging efficiencies and an (optional)
     * name of directory with efficiency histograms in the file. The file name is resolved with
     * the help of FileInPath class adding a postfix "BTag/" to the standard location. If the file
     * is not found, an exception is thrown.
     */
    BTagEffService(std::string const &name, std::string const &fileName,
      std::string const &directory = "");
    
    /// A short-cut for the above version with a default name "BTagEff"
    BTagEffService(std::string const &fileName, std::string const &directory = "");
    
    /**
     * \brief Copy constructor
     * 
     * The source file and efficiency histograms are shared with src. Although copying is expected
     * to be done before the first call to BeginRun only, the constructor is expected work
     * correctly at any moment.
     */
    BTagEffService(BTagEffService const &src);
    
    /// Default move constructor
    BTagEffService(BTagEffService &&) = default;
    
    /// Assignment operator is deleted
    BTagEffService &operator=(BTagEffService const &) = delete;
    
    /// Trivial virtual destructor
    virtual ~BTagEffService() noexcept;
    
public:
    /**
     * \brief Creates a newly configured clone
     * 
     * Implemented from Service.
     */
    virtual Service *Clone() const override;
    
    /**
     * \brief Loads b-tagging efficiencies for the given dataset
     * 
     * The method must be executed before the first call to GetEfficiency and after the last call to
     * SetProcessLabel. It reads from the source file all histograms available for the given
     * dataset. If none of process codes assigned to the dataset is mapped to a text process label
     * with the help of SetProcessLabel method, the default process label is used. If the default
     * label is neither available, an exception is thrown.
     * 
     * In order to find the process label, mapping rules defined by SetProcessLabel are examined
     * in the order of their specification. A rule is accepted if dataset.TestProcess returns true
     * when applied to the process code mentioned in the rule.
     * 
     * If an expected histogram is not found in the source file, it is skipped. The method does not
     * check wheather a meaningful set of histograms has been read. Instead, it delegates such
     * examination to the GetEfficiency method.
     * 
     * Reimplemented from Service.
     */
    virtual void BeginRun(Dataset const &dataset) override;
    
    /**
     * \brief Returns b-tagging efficiency for the given working point and given jet
     * 
     * The efficiency is read from a histogram selected according to jet flavour and requested
     * working point. Appropriate bin of the histogram is identified by jet transverse momentum and
     * (signed) pseudorapidity; overflow bins are expected to be filled in a meaningful way. If
     * required histogram is not found, an exception is thrown.
     */
    double GetEfficiency(BTagger::WorkingPoint wp, Jet const &jet) const;
    
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
    /// Opens input file and adjusts name of the in-file directory
    void OpenInputFile(std::string const &fileName);
    
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
    
    /**
     * \brief Map with b-tagging efficiency histograms
     * 
     * The map key is a pair of desired working point and absolute value of jet flavour.
     */
    std::map<std::pair<BTagger::WorkingPoint, int>, std::shared_ptr<TH2>> effHists;
};

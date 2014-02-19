/**
 * \file BTagEfficiencies.hpp
 * \author Andrey Popov
 * 
 * Defines a class to access b-tagging efficiencies.
 */

#pragma once

#include <BTagEffInterface.hpp>

#include <Dataset.hpp>

#include <TFile.h>
#include <TH2D.h>

#include <string>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <utility>


/**
 * \class BTagEfficiencies
 * \brief Accesses b-tagging efficiencies stored in a ROOT file
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
 * file and histograms with b-tagging efficiences can be shared among several threads.
 * 
 * Consult documentation on the base class to get additional details about overriden methods.
 */
class BTagEfficiencies: public BTagEffInterface
{
public:
    /**
     * \brief Constructor
     * 
     * The arguments are a path to a ROOT file with b-tagging efficiencies and an (optional)
     * name of directory with efficiency histograms in the file. The file name is resolved with
     * the help of FileInPath class adding a postfix "BTag/" to the standard location. If the file
     * is not found, an exception is thrown.
     */
    BTagEfficiencies(std::string const &fileName, std::string const &directory = "");
    
    /**
     * \brief Copy constructor
     * 
     * The source file and efficiency histograms are shared with src. Although copying is expected
     * to be done before the first call to LoadPayload only, the constructor should work correctly
     * at any moment.
     */
    BTagEfficiencies(BTagEfficiencies const &src);
    
    /// Move constructor
    BTagEfficiencies(BTagEfficiencies &&src);
    
    /**
     * \brief Assignment operator
     * 
     * The source file and efficiency histograms are shared with rhs.
     */
    BTagEfficiencies &operator=(BTagEfficiencies const &rhs);
    
public:
    /**
     * \brief Specifies a text label to which the given process code should be mapped
     * 
     * The label is later used to construct the name of a histogram with efficiencies as described
     * in the documentation of the class. The method can be called several times to specify mapping
     * of different process codes. The mapping rules are stored in the order of specification. If
     * the method is called for a process code that is already mapped, the corresponding label is
     * updated.
     */
    void SetProcessLabel(Dataset::Process code, std::string const &label);
    
    /**
     * \brief Sequentially calls the overload with a single process code for all provided values,
     * in the order of their appearance.
     */
    void SetProcessLabel(std::list<Dataset::Process> const &codes, std::string const &label);
    
    /// Sets a default label to be used when no label is specied for a process code
    void SetDefaultProcessLabel(std::string const &label);
    
    /// Returns a newly allocated copy of *this created with the copy constructor
    virtual BTagEffInterface *Clone() const;
    
    /**
     * \brief Loads b-tagging efficiencies for a given dataset
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
     */
    virtual void LoadPayload(Dataset const &dataset);
    
    /**
     * \brief Returns b-tagging efficiency for a given working point and a given jet
     * 
     * The efficiency is read from a histogram selected according to jet flavour and requested
     * working point. Appropriate bin of the histogram is identified by jet transverse momentum and
     * absolute value of pseudorapidity; overflow bins are expected to be filled in a meaningful
     * way. If required histogram is not found, an exception is thrown.
     */
    virtual double GetEfficiency(BTagger::WorkingPoint wp, Jet const &jet) const;
    
    /**
     * \brief Returns a text label for a b-tagging working point
     * 
     * The label is used to build names of histograms with b-tagging efficiencies.
     */
    static std::string WorkingPointToText(BTagger::WorkingPoint wp);
    
private:
    /**
     * \brief Source ROOT file with b-tagging efficiencies
     * 
     * The file is shared among all copies of *this.
     */
    std::shared_ptr<TFile> srcFile;
    
    /// Directory in the source ROOT file that contains histograms with b-tagging efficiencies
    std::string inFileDirectory;
    
    /**
     * \brief All process labels registered with the help of SetProcessLabel
     * 
     * Labels are the parts of efficiency histograms' names that depend on process. The labels are
     * accessed by index, which is stored in processMap.
     */
    std::vector<std::string> processLabels;
    
    /**
     * \brief Correspondance between process codes and indices in container processLabels
     * 
     * The container is sorted in process codes in the order of their appearance (and for this
     * reason cannot be implemented as an std::map). Process codes must not repeat. The second value
     * in the pair is an index of process label in the container processLabels. The label is not
     * stored directly here in order to save memory as many process codes are expected to refer to
     * the same label.
     * 
     * An alternative to deal with tightly connected processLabels and processMap would be to save
     * process labels with the help of shared pointers as it is done for efficiency histograms.
     */
    std::list<std::pair<Dataset::Process, int>> processMap;
    
    /// Label to be used with process codes for which no mapping rule is defined in processMap
    std::string defaultProcessLabel;
    
    /**
     * \brief Map with b-tagging efficiency histograms
     * 
     * The map key is a pair of desired working point and absolute value of jet flavour.
     */
    std::map<std::pair<BTagger::WorkingPoint, int>, std::shared_ptr<TH2>> effHists;
};

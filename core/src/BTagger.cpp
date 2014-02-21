#include <BTagger.hpp>

#include <BTagSFInterface.hpp>

#include <stdexcept>
#include <sstream>


using namespace std;


BTagger::BTagger(Algorithm algo_, WorkingPoint defaultWP_ /*= WorkingPoint::Tight*/):
    algo(algo_), defaultWP(defaultWP_),
    bTagMethod(nullptr)
{
    // Set thresholds corresponding to official working points [1]
    //[1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagPerformanceOP
    switch (algo)
    {
        case Algorithm::CSV:
            thresholds[WorkingPoint::Tight] = 0.898;
            thresholds[WorkingPoint::Medium] = 0.679;
            thresholds[WorkingPoint::Loose] = 0.244;
            break;
        
        case Algorithm::JP:
            thresholds[WorkingPoint::Tight] = 0.790;
            thresholds[WorkingPoint::Medium] = 0.545;
            thresholds[WorkingPoint::Loose] = 0.275;
            break;
        
        case Algorithm::TCHP:
            thresholds[WorkingPoint::Tight] = 3.41;
            break;
        
        case Algorithm::CSVV1:
        case Algorithm::CSVSLV1:
            throw logic_error("BTagger::BTagger: Algorithms CSVV1 and CSVSLV1 are not supported "
             "yet.");
    }
    
    
    // Set the pointer to the method to assess the b-tagging discriminator's value
    switch (algo)
    {
        case Algorithm::CSV:
            bTagMethod = &Jet::CSV;
            break;
        
        case Algorithm::JP:
            bTagMethod = &Jet::JP;
            break;
        
        case Algorithm::TCHP:
            bTagMethod = &Jet::TCHP;
            break;
        
        case Algorithm::CSVV1:
        case Algorithm::CSVSLV1:
            throw logic_error("BTagger::BTagger: Algorithms CSVV1 and CSVSLV1 are not supported "
             "yet.");
    }
}


BTagger::BTagger(BTagger const &src):
    algo(src.algo),
    defaultWP(src.defaultWP),
    thresholds(src.thresholds),
    bTagMethod(src.bTagMethod)
{}


BTagger::BTagger(BTagger &&src):
    algo(src.algo),
    defaultWP(src.defaultWP),
    thresholds(move(src.thresholds)),
    bTagMethod(src.bTagMethod)
{}


BTagger &BTagger::operator=(BTagger const &rhs)
{
    algo = rhs.algo;
    defaultWP = rhs.defaultWP;
    thresholds = rhs.thresholds;
    bTagMethod = rhs.bTagMethod;
    
    return *this;
}


bool BTagger::IsTagged(WorkingPoint wp, Jet const &jet) const
{
    // First, check the jet pseudorapidity makes sense
    if (fabs(jet.Eta()) > BTagSFInterface::GetMaxPseudorapidity())
        // There is a very small number of tagged jets with |eta| just above 2.4
        return false;
    
    
    // Find the threshold for the given working point
    auto thresholdIt = thresholds.find(wp);
    
    if (thresholdIt == thresholds.end())
    {
        ostringstream ost;
        ost << "BTagger::IsTagged: Working point " << int(wp) << " is not supported for "
         "b-tagger " << int(algo) << ".";
        
        throw runtime_error(ost.str());
    }
    
    
    // Compare discriminator value with the threshold
    return ((jet.*bTagMethod)() > thresholdIt->second);
}


bool BTagger::IsTagged(Jet const &jet) const
{
    return IsTagged(defaultWP, jet);
}


bool BTagger::operator()(WorkingPoint wp, Jet const &jet) const
{
    return IsTagged(wp, jet);
}


bool BTagger::operator()(Jet const &jet) const
{
    return IsTagged(defaultWP, jet);
}


BTagger::Algorithm BTagger::GetAlgorithm() const
{
    return algo;
}


BTagger::WorkingPoint BTagger::GetDefaultWorkingPoint() const
{
    return defaultWP;
}


BTagger::WorkingPoint BTagger::GetWorkingPoint() const
{
    return defaultWP;
}

string BTagger::GetTextCode() const
{
    string code;
    
    switch (algo)
    {
        case Algorithm::CSV:
            code += "CSV";
            break;
        
        case Algorithm::JP:
            code += "JP";
            break;
        
        case Algorithm::TCHP:
            code += "TCHP";
            break;
        
        case Algorithm::CSVV1:
            code += "CSVV1";
            break;
        
        case Algorithm::CSVSLV1:
            code += "CSVSLV1";
            break;
    }
    
    switch (defaultWP)
    {
        case WorkingPoint::Tight:
            code += "T";
            break;
        
        case WorkingPoint::Medium:
            code += "M";
            break;
        
        case WorkingPoint::Loose:
            code += "L";
            break;
    }
    
    
    return code;
}

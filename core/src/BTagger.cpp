#include <BTagger.hpp>

#include <stdexcept>


using namespace std;


BTagger::BTagger(Algorithm algo_, WorkingPoint workingPoint_):
    algo(algo_), workingPoint(workingPoint_),
    bTagMethod(nullptr)
{
    // Set the threshold [1]
    //[1] https://twiki.cern.ch/twiki/bin/viewauth/CMS/BTagPerformanceOP
    switch (algo)
    {
        case Algorithm::CSV:
            if (workingPoint == WorkingPoint::Tight)
                threshold = 0.898;
            else if (workingPoint == WorkingPoint::Medium)
                threshold = 0.679;
            else
                threshold = 0.244;
            
            break;
        
        case Algorithm::JP:
            if (workingPoint == WorkingPoint::Tight)
                threshold = 0.790;
            else if (workingPoint == WorkingPoint::Medium)
                threshold = 0.545;
            else
                threshold = 0.275;
            
            break;
        
        case Algorithm::TCHP:
            if (workingPoint == WorkingPoint::Tight)
                threshold = 3.41;
            else
                throw runtime_error("BTagger::BTagger: Only tight working point is supported for "
                 "TCHP b-tagging algorithm");
            
            break;
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
    }
}


bool BTagger::IsTagged(Jet const &jet) const
{
    if (fabs(jet.Eta()) > 2.4)
        // There is a very small number of tagged jets with |eta| just above 2.4
        return false;
    else
        return ((jet.*bTagMethod)() > threshold);
}


bool BTagger::operator()(Jet const &jet) const
{
    return IsTagged(jet);
}


BTagger::Algorithm BTagger::GetAlgorithm() const
{
    return algo;
}


BTagger::WorkingPoint BTagger::GetWorkingPoint() const
{
    return workingPoint;
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
    }
    
    switch (workingPoint)
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

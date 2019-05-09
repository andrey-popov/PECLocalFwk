#include <mensura/BTagger.hpp>

#include <stdexcept>


using namespace std::literals::string_literals;


// A static constant attribute
unsigned const BTagger::numWP;


BTagger::BTagger(Algorithm algo_, WorkingPoint wp_):
    algo(algo_), wp(wp_)
{}


std::string BTagger::AlgorithmToTextCode(Algorithm algo)
{
    switch (algo)
    {
        case Algorithm::CSV:
            return "CSVv2";
        
        case Algorithm::JP:
            return "JP";
        
        case Algorithm::CMVA:
            return "cMVAv2";
        
        case Algorithm::DeepCSV:
            return "DeepCSV";
        
        default:
            throw std::runtime_error("BTagger::AlgorithmToTextCode: Unsupported algorithm code "s +
              std::to_string(unsigned(algo)) + ".");
    }
}


BTagger::Algorithm BTagger::GetAlgorithm() const
{
    return algo;
}


std::string BTagger::GetTextCode() const
{
    return AlgorithmToTextCode(algo) + "_" + WorkingPointToTextCode(wp);;
}


BTagger::WorkingPoint BTagger::GetWorkingPoint() const
{
    return wp;
}


std::size_t BTagger::Hash() const
{
    return unsigned(algo) * BTagger::numWP + unsigned(wp);
}


bool BTagger::operator==(BTagger const &other) const
{
    return (other.algo == algo and other.wp == wp);
}


std::string BTagger::WorkingPointToTextCode(WorkingPoint wp)
{
    switch (wp)
    {
        case WorkingPoint::Tight:
            return "T";
        
        case WorkingPoint::Medium:
            return "M";
        
        case WorkingPoint::Loose:
            return "L";
        
        default:
            throw std::runtime_error("BTagger::WorkingPointToTextCode: Unsupported working "s +
              "point code " + std::to_string(unsigned(wp)) + ".");
    }
}

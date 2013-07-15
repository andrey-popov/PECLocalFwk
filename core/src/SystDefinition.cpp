#include <SystDefinition.hpp>

#include <stdexcept>
#include <limits>


using namespace std;


SystVariation::SystVariation():
    type(SystTypeAlgo::None), direction(0)
{}


SystVariation::SystVariation(SystTypeAlgo type, int direction)
{
    Set(type, direction);
}


void SystVariation::Set(SystTypeAlgo type_, int direction_)
{
    // Check if value of direction is reasonable
    if (abs(direction_) not_eq 1 and direction_ not_eq 0)
        throw logic_error("SystVariation::Set: Only values +1, -1, and 0 are supported for "
         "a direction of a systematical variation.");
    
    if (type_ == SystTypeAlgo::None or type_ == SystTypeAlgo::WeightOnly)
    {
        if (direction_ not_eq 0)
            throw logic_error("SystVariation::Set: Systematical uncertainties SystTypeAlgo::None "
             "and SystTypeAlgo::WeightOnly can be used with direction 0 only.");
    }
    else
    {
        if (direction_ == 0)
            throw logic_error("SystVariation::Set: Direction 0 is supported only for systematical "
             "uncertainties of types SystTypeAlgo::None and SystTypeAlgo::WeightOnly.");
    }
    
    type = type_;
    direction = direction_;
}


WeightPair::WeightPair():
    up(numeric_limits<double>::max()), down(-numeric_limits<double>::max())
{}


WeightPair::WeightPair(double up_, double down_):
    up(up_), down(down_)
{}


void WeightPair::Set(double up_, double down_)
{
    up = up_;
    down = down_;
}

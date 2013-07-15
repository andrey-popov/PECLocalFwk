#include <WeightPileUpInterface.hpp>


WeightPileUpInterface::Weights::Weights():
    central(0.),
    up(0.),
    down(0.)
{}


WeightPileUpInterface::Weights::Weights(double central_, double up_, double down_):
    central(central_),
    up(up_),
    down(down_)
{}


void WeightPileUpInterface::Weights::Set(double central_, double up_, double down_)
{
    central = central_;
    up = up_;
    down = down_;
}


void WeightPileUpInterface::SetDataset(Dataset const &)
{}
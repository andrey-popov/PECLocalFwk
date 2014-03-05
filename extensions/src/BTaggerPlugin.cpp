#include <BTaggerPlugin.hpp>


using namespace std;


BTaggerPlugin::BTaggerPlugin(string const &name_) noexcept:
    Plugin(name_)
{}


BTaggerPlugin::BTaggerPlugin(BTaggerPlugin const &src) noexcept:
    Plugin(src.name)
{}


BTaggerPlugin::~BTaggerPlugin() noexcept
{}


bool BTaggerPlugin::ProcessEvent()
{
    return true;
}

#include <BTaggerPlugin.hpp>


using namespace std;


BTaggerPlugin::BTaggerPlugin(string const &name_) noexcept:
    AnalysisPlugin(name_)
{}


BTaggerPlugin::BTaggerPlugin(BTaggerPlugin const &src) noexcept:
    AnalysisPlugin(src.name)
{}


BTaggerPlugin::~BTaggerPlugin() noexcept
{}


bool BTaggerPlugin::ProcessEvent()
{
    return true;
}

#include <PECFwk/core/Plugin.hpp>


using namespace std;


Plugin::Plugin(string const &name_):
    name(name_), master(nullptr)
{}


Plugin::~Plugin()
{}


void Plugin::SetMaster(Processor const *processor)
{
    master = processor;
}


string const &Plugin::GetName() const
{
    return name;
}


void Plugin::BeginRun(Dataset const &)
{}


void Plugin::EndRun()
{}


Plugin::EventOutcome Plugin::ProcessEventToOutcome()
{
    return ReinterpretDecision(ProcessEvent());
}

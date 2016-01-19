#include <Plugin.hpp>


using namespace std;


Plugin::Plugin(string const &name_):
    name(name_), processor(nullptr)
{}


Plugin::~Plugin()
{}


void Plugin::SetParent(Processor const *processor_)
{
    processor = processor_;
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

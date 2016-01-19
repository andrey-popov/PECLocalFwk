#include <Service.hpp>


Service::Service(std::string const &name_):
    name(name_), master(nullptr)
{}


Service::~Service()
{}


void Service::SetMaster(Processor const *processor)
{
    master = processor;
}


std::string const &Service::GetName() const
{
    return name;
}


void Service::BeginRun(Dataset const &)
{}


void Service::EndRun()
{}

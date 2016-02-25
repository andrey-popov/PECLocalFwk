#include <PECFwk/core/Service.hpp>

#include <stdexcept>


Service::Service(std::string const &name_):
    name(name_), master(nullptr)
{}


Service::~Service()
{}




void Service::BeginRun(Dataset const &)
{}


void Service::EndRun()
{}


std::string const &Service::GetName() const
{
    return name;
}


Processor const &Service::GetMaster() const
{
    if (not master)
    {
        throw std::logic_error("Service::GetMaster: The service does not have a master.");
    }
    
    return *master;
}


void Service::SetMaster(Processor const *processor)
{
    master = processor;
}

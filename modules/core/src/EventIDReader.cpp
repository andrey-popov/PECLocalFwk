#include <mensura/core/EventIDReader.hpp>


EventIDReader::EventIDReader(std::string const name /*= "EventID"*/):
    ReaderPlugin(name)
{}


EventIDReader::~EventIDReader() noexcept
{}


EventID const &EventIDReader::GetEventID() const
{
    return eventID;
}

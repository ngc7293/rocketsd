#include "clients/client.h"

namespace clients {

Client::Client(QObject* parent, ProtocolSP protocol)
    : QObject(parent)
    , protocol_(protocol)
{
}

Client::~Client() {}

} // namespace
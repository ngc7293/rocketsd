#ifndef CLIENT_FACTORY_H_
#define CLIENT_FACTORY_H_

#include "client.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace clients {

class ClientFactory {
public:
    static Client* build(ProtocolSP protocol, json& config, QObject* parent = nullptr);
};

}

#endif
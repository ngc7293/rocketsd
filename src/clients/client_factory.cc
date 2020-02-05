#include "clients/client_factory.h"

#include <string>

#include "log.h"

#include "clients/cute_client.h"
#include "clients/influx_client.h"

namespace clients {

Client* ClientFactory::build(ProtocolSP protocol, json& config, QObject* parent)
{
    Client* client;
    if (!(config.count("client") && config["client"].is_string())) {
        Log::warn("ClientFactory") << "Missing or invalid mandatory configuration 'client'" << std::endl;
        return nullptr;
    }

    std::string type = config["client"].get<std::string>();
    if (type == "cute") {
        client = new CuteClient(parent, protocol);
    } else if (type == "influx") {
        client = new InfluxClient(parent, protocol);
    } else {
        Log::warn("ClientFactory") << "Unknown client type '" << type << "'" << std::endl;
        return nullptr;
    }

    if (!client->init(config)) {
        return nullptr;
    }

    return client;
}

} // namespace
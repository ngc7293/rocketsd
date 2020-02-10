#include "producers/producer_factory.h"

#include <string>

#include "log.h"

#include "producers/fake_producer.h"
#include "producers/serial_producer.h"

namespace producers {

Producer* ProducerFactory::build(json& config, QObject* parent)
{
    if (!(config.count("producer") && config["producer"].is_string())) {
        Log::warn("ProducerFactory") << "Missing or invalid mandatory configuration 'producer'" << std::endl;
        return nullptr;
    }

    std::string type;
    type = config["producer"].get<std::string>();

    Producer* producer;
    if (type == "fake") {
        producer = new FakeProducer(parent);
    } else if (type == "serial") {
        producer = new SerialProducer(parent);
    } else {
        Log::warn("ProducerFactory") << "Unknown producer type '" << type << "'" << std::endl;
        return nullptr;
    }

    if (!producer->init(config)) {
        delete producer;
        return nullptr;
    }

    return producer;
}

}

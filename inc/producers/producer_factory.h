#ifndef PRODUCER_FACTORY_H_
#define PRODUCER_FACTORY_H_

#include "producer.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace producers {

class ProducerFactory {
public:
    static Producer* build(json& config, QObject* parent = nullptr);
};

}

#endif
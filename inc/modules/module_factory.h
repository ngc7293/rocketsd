#ifndef MODULE_FACTORY_H_
#define MODULE_FACTORY_H_

#include "module.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace modules {

class ModuleFactory {
public:
    static Module* build(ProtocolSP protocol, json& config, QObject* parent = nullptr);
};

}

#endif
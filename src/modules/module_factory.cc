#include "modules/module_factory.h"

#include <string>

#include "log.h"
#include "util.h"

#include "modules/cute_module.h"
#include "modules/influx_module.h"
#include "modules/fake_module.h"
#include "modules/serial_module.h"

namespace modules {

Module* ModuleFactory::build(ProtocolSP protocol, json& config, QObject* parent)
{
    Module* module;
    if (!has_string(config, "module")) {
        Log::err("ModuleFactory") << "Missing or invalid mandatory configuration 'module'" << std::endl;
        return nullptr;
    }

    std::string type = config["module"].get<std::string>();
    if (type == "cute") {
        module = new CuteModule(parent, protocol);
    } else if (type == "influx") {
        module = new InfluxModule(parent, protocol);
    } else if (type == "fake") {
        module = new FakeModule(parent, protocol);
    } else if (type == "serial") {
        module = new SerialModule(parent, protocol);
    } else {
        Log::warn("ModuleFactory") << "Unknown module type '" << type << "'" << std::endl;
        return nullptr;
    }

    if (!module->init(config)) {
        delete module;
        return nullptr;
    }

    return module;
}

} // namespace
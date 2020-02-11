#include "modules/module.h"

namespace modules {

Module::Module(QObject* parent, ProtocolSP protocol)
    : QObject(parent)
    , protocol_(protocol)
{
}

Module::~Module() {}

} // namespace
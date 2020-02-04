#include "deserializer.h"

Deserializer::Deserializer(ProtocolSP protocol)
    : protocol_(protocol)
{
}

Deserializer::~Deserializer() {}
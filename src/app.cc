#include "app.h"

#include "protocol_parser.h"
#include "cute_deserializer.h"
#include "bullshiter.h"

App::App(int argc, char* argv[])
    : QCoreApplication(argc, argv)
{
    ProtocolParser parser;
    protocol_ = ProtocolSP(parser.parse("protocol.xml"));

    if (protocol_) {
        CuteDeserializer* deserializer = new CuteDeserializer(protocol_);
        Bullshiter* bullshiter = new Bullshiter();

        bullshiter->setParent(this);
        deserializer->setParent(this);

        connect(bullshiter, &Bullshiter::packetReady, deserializer, &CuteDeserializer::deserialize);
    }
}

App::~App()
{
}
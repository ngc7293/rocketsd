#include "app.h"

#include "protocol_parser.h"
#include "cute_deserializer.h"
#include "influx_deserializer.h"
#include "bullshiter.h"

App::App(int argc, char* argv[])
    : QCoreApplication(argc, argv)
{
    ProtocolParser parser;
    protocol_ = ProtocolSP(parser.parse("protocol.xml"));

    if (protocol_) {
        Deserializer* cute = new CuteDeserializer(protocol_);
        Deserializer* influx = new InfluxDeserializer(protocol_);
        Bullshiter* bullshiter = new Bullshiter();

        bullshiter->setParent(this);
        cute->setParent(this);
        influx->setParent(this);

        connect(bullshiter, &Bullshiter::packetReady, cute, &Deserializer::deserialize);
        connect(bullshiter, &Bullshiter::packetReady, influx, &Deserializer::deserialize);
    }
}

App::~App()
{
}
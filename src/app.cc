#include "app.h"

#include <fstream>

#include <nlohmann/json.hpp>

#include "protocol_parser.h"
#include "clients/client_factory.h"
#include "bullshiter.h"

using json = nlohmann::json;

App::App(int argc, char* argv[])
    : QCoreApplication(argc, argv)
{
    ProtocolParser parser;
    protocol_ = ProtocolSP(parser.parse("protocol.xml"));

    if (protocol_) {
        Bullshiter* bullshit = new Bullshiter();
        bullshit->setParent(this);

        std::ifstream ifs("config.json");
        json config = json::parse(ifs);

        for (auto& subconfig : config["clients"]) {
            clients::Client* client = clients::ClientFactory::build(protocol_, subconfig, this);
            if (client) {
                connect(bullshit, &Bullshiter::packetReady, client, &clients::Client::handle);
            }
        }
    }
}

App::~App()
{
}
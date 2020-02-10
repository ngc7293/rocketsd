#include "app.h"

#include <fstream>

#include <QThread>

#include <nlohmann/json.hpp>

#include "protocol_parser.h"
#include "clients/client_factory.h"
#include "producers/producer_factory.h"

using json = nlohmann::json;

Q_DECLARE_METATYPE(radio_packet_t);

App::App(int argc, char* argv[])
    : QCoreApplication(argc, argv)
{
    qRegisterMetaType<radio_packet_t>();
    ProtocolParser parser;
    protocol_ = ProtocolSP(parser.parse("protocol.xml"));

    if (protocol_) {

        std::ifstream ifs("config.json");
        json config = json::parse(ifs);

        producers::Producer* producer = producers::ProducerFactory::build(config["producer"], this);

        for (auto& subconfig : config["clients"]) {
            if (clients::Client* client = clients::ClientFactory::build(protocol_, subconfig)) {
                QThread* thread = new QThread(this);
                thread->setObjectName("Client thread");
                client->moveToThread(thread);
                connect(producer, &producers::Producer::packetReady, client, &clients::Client::handle, Qt::QueuedConnection);
                connect(thread, &QThread::finished, client, &QObject::deleteLater);
                thread->start();
            }
        }
    }
}

App::~App()
{
}
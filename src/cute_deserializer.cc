#include "cute_deserializer.h"

#include <QTimer>

#include "log.h"
#include "util.h"

CuteDeserializer::CuteDeserializer(ProtocolSP protocol)
    : Deserializer(protocol)
{
    socket_ = new QLocalSocket();
    QObject::connect(socket_, &QLocalSocket::connected, this, &CuteDeserializer::onConnected);
    QObject::connect(socket_, QOverload<QLocalSocket::LocalSocketError>::of(&QLocalSocket::error), this, &CuteDeserializer::onError);
    QObject::connect(socket_, &QLocalSocket::disconnected, this, &CuteDeserializer::connect);
    connect();
}

CuteDeserializer::~CuteDeserializer()
{
    delete socket_;
}

void CuteDeserializer::deserialize(radio_packet_t packet)
{
    Node* node;
    Message* message;

    if (socket_->state() != QLocalSocket::ConnectedState) {
        return;
    }

    if ((node = (*protocol_)[packet.node]) == nullptr) {
        Log::err("CuteDeserializer") << "Could not find Node with id=" << packet.node << ": ignoring" << std::endl;
        return;
    }

    if ((message = (*node)[packet.message_id]) == nullptr) {
        Log::err("CuteDeserializer") << "Could not find Message with id=" << packet.node << "for Node '" << node->name() << "': ignoring" << std::endl;
        return;
    }

    PacketSP cutepacket = std::make_shared<Packet>();
    cutepacket->set_source(std::string("anirniq.") + std::string(node->name()) + "." + std::string(message->name()));
    cutepacket->set_value(packet.payload.FLOAT);
    cutepacket->set_timestamp(now());

    dispatch(cutepacket);
}

void CuteDeserializer::dispatch(PacketSP packet)
{
    // TODO: Once protocol is better defined, figure out max possible size of a
    // packet and make sure it fits in this buffer. As of 2020-02-03 packet size
    // is 38 bytes.
    void* buffer[512];
    packet->SerializeToArray(buffer, 512);
    socket_->write((char*)buffer, packet->ByteSizeLong());
}

void CuteDeserializer::connect()
{
    socket_->connectToServer("/tmp/cute");
}

void CuteDeserializer::onConnected()
{
    Log::info("CuteDeserializer") << "Connected to CuteStation on /tmp/cute" << std::endl;
}

void CuteDeserializer::onError(QLocalSocket::LocalSocketError error)
{
    if (socket_->state() == QLocalSocket::ConnectedState) {
        return;
    }

    QTimer* timer = new QTimer(this);
    timer->setSingleShot(true);
    QObject::connect(timer, &QTimer::timeout, this, &CuteDeserializer::connect);
    QObject::connect(timer, &QTimer::timeout, timer, &QTimer::deleteLater);
    timer->start(1000);

    Log::warn("CuteDeserializer") << "Could not connect to CuteStation, trying again in 1 sec" << std::endl;
}
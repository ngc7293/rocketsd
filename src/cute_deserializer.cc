#include "cute_deserializer.h"

#include <QTimer>

#include "log.h"

#include <chrono>

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
        Log::err("CuteDeserializer", "Could not find Node with id=" + std::to_string(packet.node) + ": ignoring");
        return;
    }

    if ((message = (*node)[packet.message_id]) == nullptr) {
        Log::err("CuteDeserializer", "Could not find Message with id=" + std::to_string(packet.node) + "for Node '" + std::string(node->name()) + "': ignoring");
        return;
    }

    PacketSP cutepacket = std::make_shared<Packet>();
    cutepacket->set_source(std::string("anirniq.") + std::string(message->name()));
    cutepacket->set_value(packet.payload.FLOAT);
    cutepacket->set_timestamp(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count());

    dispatch(cutepacket);
}

void CuteDeserializer::dispatch(PacketSP packet)
{
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
    Log::info("CuteDeserializer", "Connected to CuteStation on /tmp/cute");
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

    Log::warn("CuteDeserializer", "Could not connect to CuteStation, trying again in 1 sec");
}
#ifndef CUTE_DESERIALIZER_H_
#define CUTE_DESERIALIZER_H_

#include "deserializer.h"

#include <QLocalSocket>

#include "proto/packet.h"

class CuteDeserializer : public Deserializer {
    Q_OBJECT

private:
    QLocalSocket* socket_;

public:
    CuteDeserializer(ProtocolSP protocol);
    ~CuteDeserializer() override;

public slots:
    void deserialize(radio_packet_t packet) override;

    void onConnected();
    void onError(QLocalSocket::LocalSocketError error);


private:
    void connect();
    void dispatch(PacketSP packet);
};

#endif
#ifndef CUTE_CLIENT_H_
#define CUTE_CLIENT_H_

#include "clients/client.h"

#include <QLocalSocket>

#include "proto/packet.h"

namespace clients {

class CuteClient: public Client {
    Q_OBJECT

private:
    QLocalSocket* socket_;
    std::string socket_path_;

public:
    CuteClient(QObject* parent, ProtocolSP protocol);
    ~CuteClient() override;

    bool init(json& config) override;

private:
    void connect();
    void dispatch(PacketSP packet);

public slots:
    void handle(radio_packet_t packet) override;

private slots:
    void onConnected();
    void onError(QLocalSocket::LocalSocketError error);
};

} // namespace

#endif
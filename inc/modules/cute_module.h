#ifndef CUTE_MODULE_H_
#define CUTE_MODULE_H_

#include "module.h"

#include <QLocalSocket>

#include "proto/packet.h"

namespace modules {

class CuteModule: public Module {
    Q_OBJECT

private:
    QLocalSocket* socket_;
    std::string socket_path_;

public:
    CuteModule(QObject* parent, ProtocolSP protocol);
    ~CuteModule() override;

    bool init(json& config) override;

    std::string type() const override { return "CuteStation"; }

private:
    void connect();
    void dispatch(PacketSP packet);

public slots:
    void onPacket(radio_packet_t packet) override;

private slots:
    void onConnected();
    void onError(QLocalSocket::LocalSocketError error);
};

} // namespace

#endif
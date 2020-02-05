#ifndef DESERIALIZER_H_
#define DESERIALIZER_H_

#include <QObject>

#include <nlohmann/json.hpp>

#include "protocol.h"
#include "shared/interfaces/radio/radio_packet.h"

using json = nlohmann::json;

namespace clients {

class Client : public QObject {
    Q_OBJECT

protected:
    ProtocolSP protocol_;

public:
    Client(QObject* parent, ProtocolSP protocol);
    virtual ~Client();

    virtual bool init(json& config) = 0;

public slots:
    virtual void handle(radio_packet_t packet) = 0;
};

} // namespace

#endif
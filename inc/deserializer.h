#ifndef DESERIALIZER_H_
#define DESERIALIZER_H_

#include <QObject>

#include "protocol.h"
#include "shared/interfaces/radio/radio_packet.h"

class Deserializer : public QObject {
    Q_OBJECT

protected:
    ProtocolSP protocol_;

public:
    Deserializer(ProtocolSP protocol);
    virtual ~Deserializer();

public slots:
    virtual void deserialize(radio_packet_t packet) = 0;
};

#endif
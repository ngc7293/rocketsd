#ifndef PRODUCER_H_
#define PRODUCER_H_

#include <QObject>

#include <nlohmann/json.hpp>

#include "shared/interfaces/radio/radio_packet.h"

using json = nlohmann::json;

namespace producers {

class Producer : public QObject {
    Q_OBJECT

public:
    Producer(QObject* parent)
        : QObject(parent)
    {
    }
    virtual ~Producer() {}

    virtual bool init(json& config) = 0;

signals:
    void packetReady(radio_packet_t packet);
};

} // namespace

#endif
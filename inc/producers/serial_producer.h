#ifndef SERIAL_PRODUCER_H_
#define SERIAL_PRODUCER_H_

#include "producer.h"

#include <deque>

#include <QtSerialPort>

namespace producers {

class SerialProducer : public Producer {
private:
    QSerialPort* serialport_;

    std::deque<std::uint8_t> buffer_;

public:
    SerialProducer(QObject* parent);
    ~SerialProducer() override;

    bool init(json& config) override;

public slots:
    void handle(radio_packet_t packet) override;

    void onData();
};

}

#endif
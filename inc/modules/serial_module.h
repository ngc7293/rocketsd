#ifndef SERIAL_MODULE_H_
#define SERIAL_MODULE_H_

#include "module.h"

#include <deque>

#include <QtSerialPort>

namespace modules {

class SerialModule : public Module {
private:
    QSerialPort* serialport_;

    std::deque<std::uint8_t> buffer_;

public:
    SerialModule(QObject* parent, ProtocolSP protocol);
    ~SerialModule() override;

    bool init(json& config) override;

    std::string type() const override { return "Serial"; }

public slots:
    void onPacket(radio_packet_t packet) override;

private slots:
    void onData();
};

}

#endif
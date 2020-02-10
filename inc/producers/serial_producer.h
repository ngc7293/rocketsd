#ifndef SERIAL_PRODUCER_H_
#define SERIAL_PRODUCER_H_

#include "producer.h"

#include <QtSerialPort>

namespace producers {

class SerialProducer : public Producer {
private:
    QSerialPort* serialport_;

public:
    SerialProducer(QObject* parent);
    ~SerialProducer() override;

    bool init(json& config) override;

public slots:
    void onData();
};

}

#endif
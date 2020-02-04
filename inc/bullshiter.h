#ifndef BULLSHITER_H_
#define BULLSHITER_H_

#include <QObject>
#include <QTimer>

#include "shared/interfaces/radio/radio_packet.h"

class Bullshiter : public QObject {
    Q_OBJECT

private:
    QTimer* timer_;
    double n_;

public:
    Bullshiter();
    ~Bullshiter();

public slots:
    void onTimeout();

signals:
    void packetReady(radio_packet_t packet);
};

#endif
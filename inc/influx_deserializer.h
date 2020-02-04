#ifndef INFLUX_DESERIALIZER_H_
#define INFLUX_DESERIALIZER_H_

#include "deserializer.h"

#include <sstream>

#include <QNetworkAccessManager>
#include <QNetworkReply>

class InfluxDeserializer : public Deserializer {
    Q_OBJECT

private:
    QNetworkAccessManager* network_;
    std::stringstream buffer_;
    int lines_;

public:
    InfluxDeserializer(ProtocolSP protocol);
    ~InfluxDeserializer() override;

public slots:
    void deserialize(radio_packet_t packet) override;

    void onError(QNetworkReply* reply);
};

#endif
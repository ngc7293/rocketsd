#ifndef INFLUX_CLIENT_H_
#define INFLUX_CLIENT_H_

#include "clients/client.h"

#include <sstream>

#include <QNetworkAccessManager>
#include <QNetworkReply>

namespace clients {

class InfluxClient : public Client {
    Q_OBJECT

private:
    QNetworkAccessManager* network_;
    std::string base_url_;
    int max_lines_;

    std::stringstream buffer_;
    int lines_;

public:
    InfluxClient(QObject* parent, ProtocolSP protocol);
    ~InfluxClient() override;

    bool init(json& config) override;

public slots:
    void handle(radio_packet_t packet) override;

    void onError(QNetworkReply* reply);
};

} // namespaces

#endif
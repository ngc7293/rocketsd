#include "clients/influx_client.h"

#include <QNetworkRequest>

#include "util.h"
#include "log.h"

namespace clients {

InfluxClient::InfluxClient(QObject* parent, ProtocolSP protocol)
    : Client(parent, protocol)
    , network_(new QNetworkAccessManager(this))
    , lines_(0)
{
    connect(network_, &QNetworkAccessManager::finished, this, &InfluxClient::onError);
}

InfluxClient::~InfluxClient() {}

bool InfluxClient::init(json& config)
{
    if (!(config.count("url") && config["url"].is_string())) {
        Log::warn("InfluxClient") << "Missing or invalid mandatory configuration 'url'" << std::endl;
        return false;
    }
    base_url_ = config["url"].get<std::string>();

    if (!(config.count("buffer_size") && config["buffer_size"].is_number_unsigned())) {
        Log::warn("InfluxClient") << "Missing or invalid mandatory configuration 'buffer_size'" << std::endl;
        return false;
    }
    max_lines_ = config["buffer_size"].get<int>();

    Log::info("InfluxClient") << "Successfully init'd InfluxStation client" << std::endl;
    return true;
}

void InfluxClient::handle(radio_packet_t packet)
{
    Node* node;
    Message* message;

    if ((node = (*protocol_)[packet.node]) == nullptr) {
        Log::err("CuteDeserializer") << "Could not find Node with id=" << packet.node << ": ignoring" << std::endl;
        return;
    }

    if ((message = (*node)[packet.message_id]) == nullptr) {
        Log::err("CuteDeserializer") << "Could not find Message with id=" << packet.node << "for Node '" << node->name() << "': ignoring" << std::endl;
        return;
    }


    std::string measurement = std::string(message->name());
    buffer_ << measurement << ",node=" << node->name() << " value=" << packet.payload.FLOAT << " " << now_ns() << std::endl;
    lines_++;

    if (lines_ >= max_lines_) {
        QNetworkRequest request(QUrl((base_url_ + "/write?db=anirniq").c_str()));
        request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/x-influx");
        network_->post(request, QByteArray(buffer_.str().c_str()));
        buffer_.str("");
        lines_ = 0;
    }
}

void InfluxClient::onError(QNetworkReply* reply)
{
    switch (reply->error()) {
        case QNetworkReply::NetworkError::NoError:
            break;

        default:
            Log::err("InfluxClient") << "HTTP error " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << std::endl;
    }
}

} // namespace
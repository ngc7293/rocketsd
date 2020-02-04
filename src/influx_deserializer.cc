#include "influx_deserializer.h"

#include <QNetworkRequest>

#include "util.h"
#include "log.h"

InfluxDeserializer::InfluxDeserializer(ProtocolSP protocol)
    : Deserializer(protocol)
    , network_(new QNetworkAccessManager(this))
    , lines_(0)
{
    connect(network_, &QNetworkAccessManager::finished, this, &InfluxDeserializer::onError);
}

InfluxDeserializer::~InfluxDeserializer() {}

void InfluxDeserializer::deserialize(radio_packet_t packet)
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

    if (lines_ > 100) {
        QNetworkRequest request(QUrl("http://localhost:8086/write?db=anirniq"));
        request.setHeader(QNetworkRequest::KnownHeaders::ContentTypeHeader, "application/influx");
        network_->post(request, QByteArray(buffer_.str().c_str()));
        buffer_.clear();
        lines_ = 0;
    }
}

void InfluxDeserializer::onError(QNetworkReply* reply)
{
    switch (reply->error()) {
        case QNetworkReply::NetworkError::NoError:
            break;

        default:
            Log::err("InfluxDeserializer") << "HTTP error " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << std::endl;
    }
}
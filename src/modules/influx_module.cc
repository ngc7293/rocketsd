#include "modules/influx_module.h"

#include <QNetworkRequest>
#include <QUrl>

#include "util.h"
#include "log.h"

namespace modules {

InfluxModule::InfluxModule(QObject* parent, ProtocolSP protocol)
    : Module(parent, protocol)
    , network_(new QNetworkAccessManager(this))
    , lines_(0)
{
    connect(network_, &QNetworkAccessManager::finished, this, &InfluxModule::onError);
}

InfluxModule::~InfluxModule() {}

bool InfluxModule::init(json& config)
{
    if (!has_string(config, "url")) {
        Log::err("InfluxModule") << "Missing or invalid mandatory configuration 'url'" << std::endl;
        return false;
    }
    base_url_ = config["url"].get<std::string>();
    if (!QUrl(base_url_.c_str()).isValid()) {
        Log::err("InfluxModule") << "Invalid URL" << std::endl;
        return false;
    }

    if (!has_uint(config, "buffer_size")) {
        Log::err("InfluxModule") << "Missing or invalid mandatory configuration 'buffer_size'" << std::endl;
        return false;
    }
    max_lines_ = config["buffer_size"].get<int>();

    Log::info("InfluxModule") << "Successfully init'd InfluxStation client" << std::endl;
    return true;
}

void InfluxModule::onPacket(radio_packet_t packet)
{
    Node* node;
    Message* message;

    if ((node = (*protocol_)[packet.node]) == nullptr) {
        Log::warn("CuteDeserializer") << "Could not find Node with id=" << packet.node << ": ignoring" << std::endl;
        return;
    }

    if ((message = (*node)[packet.message_id]) == nullptr) {
        Log::warn("CuteDeserializer") << "Could not find Message with id=" << packet.node << "for Node '" << node->name() << "': ignoring" << std::endl;
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

void InfluxModule::onError(QNetworkReply* reply)
{
    switch (reply->error()) {
        case QNetworkReply::NetworkError::NoError:
            break;

        default:
            Log::err("InfluxModule") << "HTTP error " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << std::endl;
    }
}

} // namespace
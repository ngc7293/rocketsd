#include <rocketsd/module/influx_module.hh>

#include <QNetworkRequest>
#include <QUrl>

#include <util/json.hh>
#include <util/time.hh>
#include <util/switch.hh>
#include <log/log.hh>

namespace rocketsd::modules {

InfluxModule::InfluxModule(QObject* parent, protocol::ProtocolSP protocol)
    : Module(parent, protocol)
    , network_(new QNetworkAccessManager(this))
    , lines_(0)
{
    connect(network_, &QNetworkAccessManager::finished, this, &InfluxModule::onError);
}

InfluxModule::~InfluxModule() {}

bool InfluxModule::init(json& config)
{
    if (!Module::init(config)) {
        return false;
    }

    if (!util::json::validate("InfluxModule", config,
        util::json::required(base_url_, "url"),
        util::json::required(max_lines_, "buffer_size")
    )) {
        return false;
    }

    if (!QUrl(base_url_.c_str()).isValid()) {
        logging::err("InfluxModule") << "Invalid URL" << logging::endl;
        return false;
    }

    logging::info("InfluxModule") << "Successfully init'd InfluxStation client" << logging::endl;
    return true;
}

void InfluxModule::onPacket(radio_packet_t packet)
{
    protocol::Node* node;
    protocol::Message* message;

    if ((node = (*protocol_)[packet.node]) == nullptr) {
        logging::warn("CuteDeserializer") << "Could not find Node with id=" << packet.node << ": ignoring" << logging::endl;
        return;
    }

    if ((message = (*node)[packet.message_id]) == nullptr) {
        logging::warn("CuteDeserializer") << "Could not find Message with id=" << packet.node << "for Node '" << node->name() << "': ignoring" << logging::endl;
        return;
    }


    std::string measurement = std::string(message->name());
    buffer_ << measurement << ",node=" << node->name() << " value=";
    
    util::switcher::string(std::string(message->type()), {
        {"int", [this, &packet]() {buffer_ << packet.payload.INT; }},
        {"uint", [this, &packet]() {buffer_ << packet.payload.UINT; }},
        {"float", [this, &packet]() {buffer_ << packet.payload.FLOAT; }}
    });
     
    buffer_ << " " << util::time::now<std::nano>() << std::endl;
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
            logging::err("InfluxModule") << "HTTP error " << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt() << logging::endl;
    }
}

} // namespace
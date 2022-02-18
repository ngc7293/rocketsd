#include <rocketsd/module/influx_module.hh>

#include <QNetworkRequest>
#include <QUrl>

#include <influx/influx.hh>
#include <influx/measurement.hh>
#include <util/json.hh>
#include <util/time.hh>
#include <util/switch.hh>
#include <log/log.hh>

namespace rocketsd::modules {

InfluxModule::InfluxModule(QObject* parent)
    : Module(parent)
{
}

InfluxModule::~InfluxModule() {}

bool InfluxModule::init(json& config)
{
    if (!Module::init(config)) {
        return false;
    }

    std::string host, org, bucket, token;
    if (!util::json::validate("InfluxModule", config,
        util::json::required(host, "host"),
        util::json::required(org, "org_id"),
        util::json::required(bucket, "bucket_id"),
        util::json::required(token, "token"),
        util::json::required(bufferSize_, "buffer_size")
    )) {
        return false;
    }

    auto db = influx::Influx(host, org, token);
    bucket_.reset(new influx::Bucket(db.GetBucket(bucket)));

    logging::info("InfluxModule") << "Successfully init'd InfluxStation client" << logging::tag{"id", id()} << logging::endl;
    return true;
}

void InfluxModule::onMessage(Message message)
{
    if (!bucket_) {
        return;
    }

    influx::Measurement m(message.measurement.source());

    switch (message.measurement.value_case()) {
        case cute::proto::Measurement::kBool:
            m << influx::Field("value", message.measurement.bool_());
            break;
        case cute::proto::Measurement::kNumber:
            m << influx::Field("value", message.measurement.number());
            break;
        case cute::proto::Measurement::kState:
            m << influx::Field("state", message.measurement.state());
            break;
        case cute::proto::Measurement::kString:
        m << influx::Field("value", message.measurement.string());
            break;
        case cute::proto::Measurement::VALUE_NOT_SET:
            logging::err("InfluxModule") << "Received invalid measurement, ignoring" << logging::endl;
            return;
    }
     
    *bucket_ << m;

    if (bucket_->BufferedMeasurementsCount() > bufferSize_) {
        try {
            bucket_->Flush();
        } catch (influx::InfluxError& e) {
            logging::err("InfluxModule") << "Error flushing data to Influx" << logging::tag{"response", e.what()} << logging::endl;
        } catch (...) {
            logging::err("InfluxModule") << "Unhandled error when trying to flush data to Influx" << logging::endl;
        }
    }
}

} // namespace
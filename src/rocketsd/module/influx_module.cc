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

namespace {
    std::string isodate()
    {
        return util::time::to_string(std::chrono::system_clock::now(), "%F %T");
    }
}

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
        util::json::required(token, "token"),
        util::json::required(bufferSize_, "buffer_size"),
        util::json::optional(bucket, "bucket_id", "")
    )) {
        return false;
    }

    auto db = influx::Influx(host, org, token);

    if (bucket.empty()) {
        bucket_ = db.CreateBucket("rocketsd-" + isodate(), std::chrono::years(1));
    } else {
        bucket_ = db.GetBucket(bucket);
    }

    logging::info("InfluxModule") << "Successfully init'd InfluxStation client" << logging::tag{"id", id()} << logging::endl;
    return true;
}

void InfluxModule::onMessage(Message message)
{
    if (!bucket_) {
        return;
    }

    const std::string& source = message.measurement.source();
    std::size_t start = source.find_first_of('.');
    std::size_t end = source.find_first_of('.', start + 1);

    influx::Measurement m(source.substr(start + 1, end - start - 1));

    switch (message.measurement.value_case()) {
        case cute::proto::Measurement::kBool:
            m << influx::Field(source.substr(end + 1), message.measurement.bool_());
            break;
        case cute::proto::Measurement::kNumber:
            m << influx::Field(source.substr(end + 1), message.measurement.number());
            break;
        case cute::proto::Measurement::kState:
            m << influx::Field(source.substr(end + 1), message.measurement.state());
            break;
        case cute::proto::Measurement::kString:
        m << influx::Field(source.substr(end + 1), message.measurement.string());
            break;
        case cute::proto::Measurement::VALUE_NOT_SET:
            logging::err("InfluxModule") << "Received invalid measurement, ignoring" << logging::endl;
            return;
    }

    bucket_ << m;

    if (bucket_.BufferedMeasurementsCount() > bufferSize_) {
        try {
            bucket_.Flush();
        } catch (influx::InfluxError& e) {
            logging::err("InfluxModule") << "Error flushing data to Influx" << logging::tag{"response", e.what()} << logging::endl;
        } catch (...) {
            logging::err("InfluxModule") << "Unhandled error when trying to flush data to Influx" << logging::endl;
        }
    }
}

} // namespace
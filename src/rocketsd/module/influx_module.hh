#ifndef ROCKETSD_MODULES_INFLUXMODULE_HH_
#define ROCKETSD_MODULES_INFLUXMODULE_HH_

#include <rocketsd/module/module.hh>

#include <sstream>

#include <influx/bucket.hh>

namespace rocketsd::modules {

class InfluxModule : public Module {
    Q_OBJECT

private:
    influx::Bucket bucket_;
    std::size_t bufferSize_;

public:
    InfluxModule(QObject* parent);
    ~InfluxModule() override;

    bool init(json& config) override;

    std::string type() const override { return "InfluxDB"; }

public slots:
    void onMessage(Message message) override;
};

} // namespaces

#endif
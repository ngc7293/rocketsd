#include "producers/fake_producer.h"

#include <iostream>

#include <cmath>

#include "shared/interfaces/id.h"

#include "log.h"

namespace producers {

FakeProducer::FakeProducer(QObject* parent)
    : Producer(parent)
{
    timer_ = new QTimer();
    connect(timer_, &QTimer::timeout, this, &FakeProducer::onTimeout);
    n_ = 0;
}

FakeProducer::~FakeProducer()
{
}

bool FakeProducer::init(json& config)
{
    freq_ = 0;
    if (config.count("frequency") && config["frequency"].is_number()) {
        freq_ = config["frequency"].get<double>();
    }
    if (freq_ < 0) {
        freq_ = 1000;
    }

    alpha_ = 1;
    if (config.count("alpha") && config["alpha"].is_number()) {
        alpha_ = config["alpha"].get<double>();
    }

    omega_ = 1;
    if (config.count("omega") && config["omega"].is_number()) {
        omega_ = config["omega"].get<double>();
    }

    timer_->start(1000 / freq_);
    Log::info("FakeProducer") << "Successfully init'd Fake producer" << std::endl;
    return true;
}

void FakeProducer::onTimeout()
{
    radio_packet_t packet;
    packet.node = 3;
    packet.message_id = CAN_ACQUISITION_GPS_LAT_INDEX;
    packet.payload.FLOAT = alpha_ * std::sin(n_);
    n_ += omega_ * ((M_PI * 2) / freq_);
    packetReady(packet);
}

} // namespace
#include <rocketsd/module/fake_module.hh>

#include <iostream>

#include <cmath>

#include "shared/interfaces/id.h"

#include <log/log.hh>
#include <util/json.hh>

#ifndef M_PI
#define M_PI 3.1415
#endif

namespace rocketsd::modules {

FakeModule::FakeModule(QObject* parent, protocol::ProtocolSP protocol)
    : Module(parent, protocol)
{
    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &FakeModule::onTimeout);
    n_ = 0;
}

FakeModule::~FakeModule()
{
}

bool FakeModule::init(json& config)
{
    if (!Module::init(config)) {
        return false;
    }

    if (!util::json::validate("FakeModule", config,
        util::json::optional(freq_, "frequency", 1.0),
        util::json::optional(alpha_, "alpha", 1.0),
        util::json::optional(omega_, "omega", 1.0),
        util::json::optional(nodeid_, "node_id", 0u),
        util::json::optional(messageid_, "message_id", 0u)
    )) {
        return false;
    }

    timer_->start(1000 / freq_);

    logging::info("FakeModule") << "Successfully init'd Fake producer" << logging::endl;
    return true;
}

void FakeModule::onTimeout()
{
    radio_packet_t packet;
    packet.node = nodeid_;
    packet.message_id = messageid_;
    packet.payload.FLOAT = alpha_ * std::sin(n_);
    n_ += omega_ * ((M_PI * 2) / freq_);
    emit packetReady(packet);
}

void FakeModule::onPacket(radio_packet_t packet)
{
    logging::info("FakeModule") << "Received packet: node=" << +packet.node << " message=" << +packet.message_id << " crc=" << +packet.checksum << logging::endl;
}

} // namespace
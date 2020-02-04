#include "bullshiter.h"

#include <iostream>

#include <cmath>

#include "shared/interfaces/id.h"

Bullshiter::Bullshiter()
{
    timer_ = new QTimer();
    connect(timer_, &QTimer::timeout, this, &Bullshiter::onTimeout);
    timer_->start(1);
    n_ = 0;
}

Bullshiter::~Bullshiter()
{
}

void Bullshiter::onTimeout()
{
    radio_packet_t packet;
    packet.node = 3;
    packet.message_id = CAN_ACQUISITION_GPS_LAT_INDEX;
    packet.payload.FLOAT = std::sin(n_);
    n_ += 0.05;
    packetReady(packet);
}
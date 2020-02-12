#include "modules/serial_module.h"

#include "log.h"
#include "util.h"

namespace modules {

SerialModule::SerialModule(QObject* parent, ProtocolSP protocol)
    : Module(parent, protocol)
{
}

SerialModule::~SerialModule()
{
}

bool SerialModule::init(json& config)
{

    if (!has<std::string>(config, "port")) {
        Log::err("SerialModule") << "Missing or invalid configuration 'port'" << std::endl;
        return false;
    }
    std::string port = config["port"].get<std::string>();

    if (!has<unsigned int>(config, "baudrate")) {
        Log::err("SerialModule") << "Missing or invalid configuration 'baudrate'" << std::endl;
        return false;
    }
    unsigned int baudrate = config["baudrate"].get<unsigned int>();

    serialport_ = new QSerialPort(port.c_str());
    serialport_->setBaudRate(baudrate);

    connect(serialport_, &QSerialPort::readyRead, this, &SerialModule::onData);

    Log::info("SerialModule") << "Successfully init'd Serial producer" << std::endl;
    return true;
}

void SerialModule::onPacket(radio_packet_t packet)
{
    //FIXME: This has not been tested whatsoever
    serialport_->write((char*)&packet, sizeof(radio_packet_t));
}

void SerialModule::onData()
{
    //FIXME: This has not been tested whatsoever
    radio_packet_t packet;

    for (const auto& byte : serialport_->readAll()) {
        buffer_.push_back(static_cast<std::uint8_t>(byte));

        if (buffer_.size() > sizeof(radio_packet_t)) {
            // Yes, this is very explicit/ugly, but:
            // - Can't memcpy/reinterpret_cast because dequeue is non-continuous
            // - No branch is not pretty but it is efficient, predictable
            packet.node = buffer_[0];
            packet.message_id = buffer_[1];

            packet.payload.BYTES[0] = buffer_[2];
            packet.payload.BYTES[1] = buffer_[3];
            packet.payload.BYTES[2] = buffer_[4];
            packet.payload.BYTES[3] = buffer_[5];

            packet.payload.BYTES[4] = buffer_[6];
            packet.payload.BYTES[5] = buffer_[7];
            packet.payload.BYTES[6] = buffer_[8];
            packet.payload.BYTES[7] = buffer_[9];

            packet.checksum = buffer_[10];

            if (packet.checksum == radio_compute_crc(&packet)) {
                packetReady(packet);
                buffer_.erase(buffer_.begin(), buffer_.begin() + sizeof(radio_packet_t));
            } else {
                buffer_.erase(buffer_.begin());
            }
        }
    }
}

} // namespace
#include "producers/serial_producer.h"

#include "log.h"

namespace producers {

SerialProducer::SerialProducer(QObject* parent)
    : Producer(parent)
{
}

SerialProducer::~SerialProducer()
{
}

bool SerialProducer::init(json& config)
{
    Log::info("SerialProducer") << "Successfully init'd Serial producer" << std::endl;

    if (!(config.count("port") && config["port"].is_string())) {
        Log::warn("SerialProducer") << "Missing or invalid configuration 'port'" << std::endl;
        return false;
    }
    std::string port = config["port"].get<std::string>();

    if (!(config.count("baudrate") && config["baudrate"].is_number_unsigned())) {
        Log::warn("SerialProducer") << "Missing or invalid configuration 'baudrate'" << std::endl;
        return false;
    }
    unsigned int baudrate = config["baudrate"].get<unsigned int>();

    serialport_ = new QSerialPort(port.c_str());
    serialport_->setBaudRate(baudrate);

    connect(serialport_, &QSerialPort::readyRead, this, &SerialProducer::onData);


    return true;
}

void SerialProducer::onData()
{

}

} // namespace
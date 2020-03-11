#include "log.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>

#include <ctime>

namespace {
const char* LOG_TIME_FORMAT = "%Y-%m-%d %H:%M:%S";

const std::string string_from_level(Log::Level level)
{
    static const std::map<const Log::Level, const std::string> LOG_LEVEL_STRING = {
        { Log::DEBUG, "debug" },
        { Log::INFO, "info" },
        { Log::WARNING, "warning" },
        { Log::ERROR, "error" },
    };

    return LOG_LEVEL_STRING.at(level);
}

}

std::ostream& Log::debug(std::string component, std::string message)
{
    return Log::get().log(DEBUG, component, message);
}

std::ostream& Log::info(std::string component, std::string message)
{
    return Log::get().log(INFO, component, message);
}

std::ostream& Log::warn(std::string component, std::string message)
{
    return Log::get().log(WARNING, component, message);
}

std::ostream& Log::err(std::string component, std::string message)
{
    return Log::get().log(ERROR, component, message);
}

Log::Log() : stream_(&std::cout) {}

Log::~Log() {}

Log& Log::get()
{
    static Log log;
    return log;
}

std::ostream& Log::log(Level level, std::string component, std::string message)
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    *stream_ << "[" << std::put_time(std::localtime(&now), LOG_TIME_FORMAT) << "] (" << string_from_level(level) << ") [" << component << "] ";

    if (message != "") {
        *stream_ << message << std::endl;
    }

    return *stream_;
}

#include "log.h"

#include <chrono>
#include <iomanip>
#include <iostream>
#include <map>

#include <ctime>

namespace {
const std::map<Log::Level, std::string> LOG_LEVEL_STRING = {
    { Log::DEBUG, "debug" },
    { Log::INFO, "info" },
    { Log::WARNING, "warning" },
    { Log::ERROR, "error" }
};

const char* LOG_TIME_FORMAT = "%Y-%m-%d %H:%M:%S";
}

void Log::debug(std::string component, std::string message)
{
    Log::get().log(DEBUG, component, message);
}

void Log::info(std::string component, std::string message)
{
    Log::get().log(INFO, component, message);
}

void Log::warn(std::string component, std::string message)
{
    Log::get().log(WARNING, component, message);
}

void Log::err(std::string component, std::string message)
{
    Log::get().log(ERROR, component, message);
}

Log::Log() {}

Log::~Log() {}

Log& Log::get()
{
    static Log log;
    return log;
}

void Log::log(Level level, std::string component, std::string message)
{
    auto now = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::cout << "[" << std::put_time(std::localtime(&now), LOG_TIME_FORMAT) << "] (" << LOG_LEVEL_STRING.at(level) << ") [" << component << "] " << message << std::endl;
}

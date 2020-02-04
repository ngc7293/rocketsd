#ifndef LOG_H_
#define LOG_H_

#include <string>

class Log {
public:
    enum Level {
        DEBUG = -1,
        INFO = 0,
        WARNING = 1,
        ERROR = 2
    };

public:
    static Log& get();

    static void debug(std::string component, std::string message);
    static void info(std::string component, std::string message);
    static void warn(std::string component, std::string message);
    static void err(std::string component, std::string message);

public:
    ~Log();

    void log(Level level, std::string component, std::string message);

private:
    Log();
};

#endif
#ifndef APP_H_
#define APP_H_

#include <QCoreApplication>

#include <QThread>

#include "protocol.h"

class App : public QCoreApplication {
private:
    ProtocolSP protocol_;
    std::vector<QThread*> workers_;

public:
    App(int argc, char* argv[]);
    ~App();
};

#endif
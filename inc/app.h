#ifndef APP_H_
#define APP_H_

#include <QCoreApplication>

#include "protocol.h"

class App : public QCoreApplication {
private:
    ProtocolSP protocol_;

public:
    App(int argc, char* argv[]);
    ~App();
};

#endif
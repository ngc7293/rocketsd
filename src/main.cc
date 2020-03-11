#include "app.h"
#include "proto/packet.h"

#include <signal.h>

int main(int argc, char* argv[])
{
    qRegisterMetaType<cute::proto::PacketSP>();
    App app(argc, (char**)argv);
    signal(SIGINT, [](int a){ QCoreApplication::quit(); });
    return app.exec();
}

#include "app.h"
#include "proto/packet.h"

int main(int argc, char* argv[])
{
    qRegisterMetaType<PacketSP>();
    App app(argc, (char**)argv);
    return app.exec();
}

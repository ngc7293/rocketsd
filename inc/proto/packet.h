#ifndef PROTO_PACKET_H_
#define PROTO_PACKET_H_

#include <memory>

#include "packet.pb.h"
#include <QObject>

typedef std::shared_ptr<Packet> PacketSP;
Q_DECLARE_METATYPE(PacketSP);

#endif
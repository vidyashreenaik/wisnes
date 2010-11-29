#include <tclcl.h>

#include "nsmiracle_aodv_packet.h"
#include "nsmiracle_aodv_packet_classifier.h"
#include "nsmiracle_aodv_packet_tracer.h"

packet_t PT_MAODV;
extern EmbeddedTcl NsmiracleaodvTclCode;

extern "C" int Nsmiracleaodv_Init() {
    p_info::addPacketClassifier(new MAODVPacketClassifier());
    PT_MAODV = p_info::addPacket((char *) "MAODV");
    CMUTrace::addPacketTracer(new MAODVPacketTracer());

    NsmiracleaodvTclCode.load();

    return 0;
}

extern "C" int Cygnsmiracleaodv_Init() {
    Nsmiracleaodv_Init();
}


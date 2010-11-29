#include <tclcl.h>

#include "aodvetx_packet.h"
#include "aodvetx_packet_classifier.h"
#include "aodvetx_packet_tracer.h"

extern EmbeddedTcl AodvetxTclCode;

packet_t PT_AODVETX;

extern "C" int Aodvetx_Init() {
    p_info::addPacketClassifier(new AODVETXPacketClassifier());
    PT_AODVETX = p_info::addPacket((char *) "AODVETX");
    CMUTrace::addPacketTracer(new AODVETXPacketTracer());

    AodvetxTclCode.load();

    return 0;
}

extern "C" int Cygaodvetx_Init() {
    Aodvetx_Init();
}


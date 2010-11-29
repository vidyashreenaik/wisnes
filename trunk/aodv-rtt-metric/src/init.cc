#include <tclcl.h>

#include "aodvrtt_packet.h"
#include "aodvrtt_packet_classifier.h"
#include "aodvrtt_packet_tracer.h"

extern EmbeddedTcl AodvrttTclCode;

packet_t PT_AODVRTT;

extern "C" int Aodvrtt_Init() {
    p_info::addPacketClassifier(new AODVRTTPacketClassifier());
    PT_AODVRTT = p_info::addPacket((char *) "AODVRTT");
    CMUTrace::addPacketTracer(new AODVRTTPacketTracer());

    AodvrttTclCode.load();

    return 0;
}

extern "C" int Cygaodvrtt_Init() {
    Aodvrtt_Init();
}


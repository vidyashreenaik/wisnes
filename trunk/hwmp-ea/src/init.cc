#include <tclcl.h>

#include "hwmp_packet.h"
#include "hwmp_packet_classifier.h"
#include "hwmp_packet_tracer.h"

extern EmbeddedTcl HwmpTclCode;

packet_t PT_HWMP;

extern "C" int Hwmp_Init()
{
    p_info::addPacketClassifier(new HWMPPacketClassifier());
    PT_HWMP = p_info::addPacket("HWMP");
    CMUTrace::addPacketTracer(new HWMPPacketTracer());

    HwmpTclCode.load();

    return 0;
}

extern "C" int Cyghwmp_Init()
{
    Hwmp_Init();
}


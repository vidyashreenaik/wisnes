#include "hwmp_packet.h"
#include "hwmp_packet_classifier.h"

HWMPPacketClassifier::HWMPPacketClassifier() : PacketClassifier()
{
}

HWMPPacketClassifier::~HWMPPacketClassifier()
{
}

packetClass
HWMPPacketClassifier::getClass(packet_t type)
{
    if (type == PT_HWMP) {
        return ROUTING;
    }

    return UNCLASSIFIED;
}


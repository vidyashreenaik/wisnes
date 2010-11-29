#include "aodvetx_packet.h"
#include "aodvetx_packet_classifier.h"

AODVETXPacketClassifier::AODVETXPacketClassifier() : PacketClassifier()
{
}

AODVETXPacketClassifier::~AODVETXPacketClassifier()
{
}

packetClass
AODVETXPacketClassifier::getClass(packet_t type)
{
    if (type == PT_AODVETX) {
        return ROUTING;
    }

    return UNCLASSIFIED;
}


#include "aodvrtt_packet.h"
#include "aodvrtt_packet_classifier.h"

AODVRTTPacketClassifier::AODVRTTPacketClassifier() : PacketClassifier()
{
}

AODVRTTPacketClassifier::~AODVRTTPacketClassifier()
{
}

packetClass
AODVRTTPacketClassifier::getClass(packet_t type)
{
    if (type == PT_AODVRTT) {
        return ROUTING;
    }

    return UNCLASSIFIED;
}


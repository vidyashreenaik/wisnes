#include "nsmiracle_aodv_packet.h"
#include "nsmiracle_aodv_packet.h"
#include "nsmiracle_aodv_packet_classifier.h"

MAODVPacketClassifier::MAODVPacketClassifier() :
    PacketClassifier() {
}

MAODVPacketClassifier::~MAODVPacketClassifier() {
}

packetClass MAODVPacketClassifier::getClass(packet_t type) {
    if (type == PT_MAODV) {
        return ROUTING;
    }

    return UNCLASSIFIED;
}

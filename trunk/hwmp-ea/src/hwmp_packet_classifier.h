#ifndef __HWMP_packet_classifier_h__
#define __HWMP_packet_classifier_h__

#include <packet.h>

class HWMPPacketClassifier : public PacketClassifier {
public:
    HWMPPacketClassifier();
    virtual ~HWMPPacketClassifier();
protected:
    virtual packetClass getClass(packet_t type);
};

#endif /* __HWMP_packet_classifier_h__ */

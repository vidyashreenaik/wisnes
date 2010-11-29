#ifndef __AODVETX_packet_classifier_h__
#define __AODVETX_packet_classifier_h__

#include <packet.h>

class AODVETXPacketClassifier : public PacketClassifier {
public:
    AODVETXPacketClassifier();
    virtual ~AODVETXPacketClassifier();
protected:
    virtual packetClass getClass(packet_t type);
};

#endif /* __AODVETX_packet_classifier_h__ */

#ifndef __aodvrtt_packet_classifier_h__
#define __aodvrtt_packet_classifier_h__

#include <packet.h>

class AODVRTTPacketClassifier : public PacketClassifier {
public:
    AODVRTTPacketClassifier();
    virtual ~AODVRTTPacketClassifier();
protected:
    virtual packetClass getClass(packet_t type);
};

#endif /* __aodvrtt_packet_classifier_h__ */

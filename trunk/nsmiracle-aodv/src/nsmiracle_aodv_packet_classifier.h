#ifndef __nsmiracle_aodv_packet_classifier_h__
#define __nsmiracle_aodv_packet_classifier_h__

#include <packet.h>

class MAODVPacketClassifier: public PacketClassifier {
public:
    MAODVPacketClassifier();
    virtual ~MAODVPacketClassifier();
protected:
    virtual packetClass getClass(packet_t type);
};

#endif /* __nsmiracle_aodv_packet_classifier_h__ */

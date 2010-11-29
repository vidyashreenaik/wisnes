#ifndef __nsmiracle_aodv_packet_tracer_h__
#define __nsmiracle_aodv_packet_tracer_h__

#include <cmu-trace.h>

class MAODVPacketTracer: public PacketTracer {
public:
    MAODVPacketTracer();
    virtual ~MAODVPacketTracer();
protected:
    virtual int format(Packet *p, int offset, BaseTrace *pt_, int newtrace);
private:
    void format_aodvca_packet(Packet *p, int offset, BaseTrace *pt,
            int newtrace);
};

#endif /* __nsmiracle_aodv_packet_tracer_h__ */

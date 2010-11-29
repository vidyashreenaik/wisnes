#ifndef __AODVETX_packet_tracer_h__
#define __AODVETX_packet_tracer_h__

#include <cmu-trace.h>

class AODVETXPacketTracer : public PacketTracer {
public:
    AODVETXPacketTracer();
    virtual ~AODVETXPacketTracer();
protected:
    virtual int format(Packet *p, int offset, BaseTrace *pt_, int newtrace);
private:
    void format_aodvetx_packet(Packet *p, int offset, BaseTrace *pt, int newtrace);
};

#endif /* __AODVETX_packet_tracer_h__ */

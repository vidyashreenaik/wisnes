#ifndef __HWMP_packet_tracer_h__
#define __HWMP_packet_tracer_h__

#include <cmu-trace.h>

class HWMPPacketTracer : public PacketTracer {
public:
    HWMPPacketTracer();
    virtual ~HWMPPacketTracer();
protected:
    virtual int format(Packet *p, int offset, BaseTrace *pt_, int newtrace);
private:
    void format_hwmp_packet(Packet *p, int offset, BaseTrace *pt, int newtrace);
};

#endif /* __HWMP_packet_tracer_h__ */

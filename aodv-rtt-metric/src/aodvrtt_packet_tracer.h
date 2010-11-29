#ifndef __aodvrtt_packet_tracer_h__
#define __aodvrtt_packet_tracer_h__

#include <cmu-trace.h>

class AODVRTTPacketTracer : public PacketTracer {
public:
    AODVRTTPacketTracer();
    virtual ~AODVRTTPacketTracer();
protected:
    virtual int format(Packet *p, int offset, BaseTrace *pt_, int newtrace);
private:
    void format_aodvrtt_packet(Packet *p, int offset, BaseTrace *pt, int newtrace);
};

#endif /* __aodvrtt_packet_tracer_h__ */

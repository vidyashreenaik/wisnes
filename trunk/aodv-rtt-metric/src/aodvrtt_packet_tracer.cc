#include "aodvrtt_packet.h"
#include "aodvrtt_packet_tracer.h"

AODVRTTPacketTracer::AODVRTTPacketTracer() :
PacketTracer()
{
}

AODVRTTPacketTracer::~AODVRTTPacketTracer()
{
}

int
AODVRTTPacketTracer::format(Packet *p, int offset, BaseTrace *pt, int newtrace)
{
    hdr_cmn *ch = HDR_CMN(p);
    if (ch->ptype() == PT_AODVRTT) {
        format_aodvrtt_packet(p, offset, pt, newtrace);
        return 1;
    }

    return 0;
}

void
AODVRTTPacketTracer::format_aodvrtt_packet(Packet *p, int offset, BaseTrace *pt,
        int newtrace)
{
    struct hdr_aodvrtt *hh = HDR_AODVRTT(p);
    struct hdr_aodvrtt_request *rq = HDR_AODVRTT_REQUEST(p);
    struct hdr_aodvrtt_reply *rp = HDR_AODVRTT_REPLY(p);
    struct hdr_aodvrtt_probe *rb = HDR_AODVRTT_PROBE(p);

    /*
    switch (hh->aodvrtt_type) {
    case AODVRTTTYPE_PREQ:

        if (pt->tagged()) {
            sprintf(pt->buffer() + offset,
                    "-aodvrtt:t %x -aodvrtt:h %d -aodvrtt:b %d -aodvrtt:d %d "
                    "-aodvrtt:ds %d -aodvrtt:s %d -aodvrtt:ss %d "
                    "-aodvrtt:c REQUEST ",
                    rq->aodvrtt_type,
                    rq->preq_hop_count,
                    rq->preq_src_seqno,
                    rq->preq_dst[0],
                    rq->preq_dst_seqno[0],
                    rq->preq_src,
                    rq->preq_src_seqno);
        } else if (newtrace) {
            sprintf(pt->buffer() + offset,
                    "-P aodvrtt -Pt 0x%x -Ph %d -Pb %d -Pd %d -Pds %d -Ps %d -Pss %d -Pc REQUEST ",
                    rq->aodvrtt_type,
                    rq->preq_hop_count,
                    rq->preq_src_seqno,
                    rq->preq_dst[0],
                    rq->preq_dst_seqno[0],
                    rq->preq_src,
                    rq->preq_src_seqno);


        } else {
            sprintf(pt->buffer() + offset,
                    "[0x%x %d %d [%d %d] [%d %d]] (REQUEST)",
                    rq->preq_type,
                    rq->preq_hop_count,
                    rq->preq_src_seqno,
                    rq->preq_dst,
                    rq->preq_dst_seqno,
                    rq->preq_src,
                    rq->preq_src_seqno);
        }
        break;
    case AODVRTTTYPE_PREP:
        if (pt->tagged()) {
            sprintf(pt->buffer() + offset,
                    "-aodvrtt:t %x -aodvrtt:h %d -aodvrtt:d %d -adov:ds %d "
                    "-aodvrtt:l %f -aodvrtt:c %s ",
                    rp->aodvrtt_type,
                    rp->prep_hop_count,
                    rp->prep_dst,
                    rp->prep_dst_seqno,
                    rp->prep_lifetime,
                    "PREP");
        } else if (newtrace) {
            sprintf(pt->buffer() + offset,
                    "-P aodvrtt -Pt 0x%x -Ph %d -Pd %d -Pds %d -Pl %f -Pc %s ",
                    rp->prep_type,
                    rp->prep_hop_count,
                    rp->prep_dst,
                    rp->prep_dst_seqno,
                    rp->prep_lifetime,
                    "PREP");
        } else {
            sprintf(pt->buffer() + offset,
                    "[0x%x %d [%d %d] %f] (%s)",
                    rp->prep_type,
                    rp->prep_hop_count,
                    rp->prep_dst,
                    rp->prep_dst_seqno,
                    rp->prep_lifetime,
                    "PREP");
        }
        break;
    default:
#ifdef WIN32
        fprintf(stderr,
                "CMUTrace::format_aodvrtt: invalid AODVRTT packet type\n");
#else
        fprintf(stderr,
                "%s: invalid AODVRTT packet type\n", __FUNCTION__);
#endif
        abort();
    }*/
}


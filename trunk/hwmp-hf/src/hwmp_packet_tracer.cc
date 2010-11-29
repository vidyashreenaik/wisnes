#include "hwmp_packet.h"
#include "hwmp_packet_tracer.h"

HWMPPacketTracer::HWMPPacketTracer() :
PacketTracer()
{
}

HWMPPacketTracer::~HWMPPacketTracer()
{
}

int
HWMPPacketTracer::format(Packet *p, int offset, BaseTrace *pt, int newtrace)
{
    hdr_cmn *ch = HDR_CMN(p);
    if (ch->ptype() == PT_HWMP) {
        format_hwmp_packet(p, offset, pt, newtrace);
        return 1;
    }

    return 0;
}

void
HWMPPacketTracer::format_hwmp_packet(Packet *p, int offset, BaseTrace *pt,
        int newtrace)
{
    struct hdr_hwmp *hh = HDR_HWMP(p);
    struct hdr_hwmp_preq *rq = HDR_HWMP_PREQ(p);
    struct hdr_hwmp_prep *rp = HDR_HWMP_PREP(p);

    switch (hh->hwmp_type) {
    case HWMPTYPE_PREQ:

        if (pt->tagged()) {
            sprintf(pt->buffer() + offset,
                    "-hwmp:t %x -hwmp:h %d -hwmp:b %d -hwmp:d %d "
                    "-hwmp:ds %d -hwmp:s %d -hwmp:ss %d "
                    "-hwmp:c REQUEST ",
                    rq->hwmp_type,
                    rq->preq_hop_count,
                    rq->preq_src_seqno,
                    rq->preq_dst[0],
                    rq->preq_dst_seqno[0],
                    rq->preq_src,
                    rq->preq_src_seqno);
        } else if (newtrace) {
            sprintf(pt->buffer() + offset,
                    "-P hwmp -Pt 0x%x -Ph %d -Pb %d -Pd %d -Pds %d -Ps %d -Pss %d -Pc REQUEST ",
                    rq->hwmp_type,
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
    case HWMPTYPE_PREP:
        if (pt->tagged()) {
            sprintf(pt->buffer() + offset,
                    "-hwmp:t %x -hwmp:h %d -hwmp:d %d -adov:ds %d "
                    "-hwmp:l %f -hwmp:c %s ",
                    rp->hwmp_type,
                    rp->prep_hop_count,
                    rp->prep_dst,
                    rp->prep_dst_seqno,
                    rp->prep_lifetime,
                    "PREP");
        } else if (newtrace) {
            sprintf(pt->buffer() + offset,
                    "-P hwmp -Pt 0x%x -Ph %d -Pd %d -Pds %d -Pl %f -Pc %s ",
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
                "CMUTrace::format_hwmp: invalid HWMP packet type\n");
#else
        fprintf(stderr,
                "%s: invalid HWMP packet type\n", __FUNCTION__);
#endif
        abort();
    }
}


#include "aodvetx_packet.h"
#include "aodvetx_packet_tracer.h"

AODVETXPacketTracer::AODVETXPacketTracer() :
PacketTracer()
{
}

AODVETXPacketTracer::~AODVETXPacketTracer()
{
}

int
AODVETXPacketTracer::format(Packet *p, int offset, BaseTrace *pt, int newtrace)
{
    hdr_cmn *ch = HDR_CMN(p);
    if (ch->ptype() == PT_AODVETX) {
        format_aodvetx_packet(p, offset, pt, newtrace);
        return 1;
    }

    return 0;
}

void
AODVETXPacketTracer::format_aodvetx_packet(Packet *p, int offset, BaseTrace *pt,
        int newtrace)
{
    struct hdr_aodvetx *hh = HDR_AODVETX(p);
    struct hdr_aodvetx_request *rq = HDR_AODVETX_REQUEST(p);
    struct hdr_aodvetx_reply *rp = HDR_AODVETX_REPLY(p);
    struct hdr_aodvetx_probe *rb = HDR_AODVETX_PROBE(p);

    /*
    switch (hh->aodvetx_type) {
    case AODVETXTYPE_PREQ:

        if (pt->tagged()) {
            sprintf(pt->buffer() + offset,
                    "-aodvetx:t %x -aodvetx:h %d -aodvetx:b %d -aodvetx:d %d "
                    "-aodvetx:ds %d -aodvetx:s %d -aodvetx:ss %d "
                    "-aodvetx:c REQUEST ",
                    rq->aodvetx_type,
                    rq->preq_hop_count,
                    rq->preq_src_seqno,
                    rq->preq_dst[0],
                    rq->preq_dst_seqno[0],
                    rq->preq_src,
                    rq->preq_src_seqno);
        } else if (newtrace) {
            sprintf(pt->buffer() + offset,
                    "-P aodvetx -Pt 0x%x -Ph %d -Pb %d -Pd %d -Pds %d -Ps %d -Pss %d -Pc REQUEST ",
                    rq->aodvetx_type,
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
    case AODVETXTYPE_PREP:
        if (pt->tagged()) {
            sprintf(pt->buffer() + offset,
                    "-aodvetx:t %x -aodvetx:h %d -aodvetx:d %d -adov:ds %d "
                    "-aodvetx:l %f -aodvetx:c %s ",
                    rp->aodvetx_type,
                    rp->prep_hop_count,
                    rp->prep_dst,
                    rp->prep_dst_seqno,
                    rp->prep_lifetime,
                    "PREP");
        } else if (newtrace) {
            sprintf(pt->buffer() + offset,
                    "-P aodvetx -Pt 0x%x -Ph %d -Pd %d -Pds %d -Pl %f -Pc %s ",
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
                "CMUTrace::format_aodvetx: invalid AODVETX packet type\n");
#else
        fprintf(stderr,
                "%s: invalid AODVETX packet type\n", __FUNCTION__);
#endif
        abort();
    }*/
}


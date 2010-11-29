#include "nsmiracle_aodv_packet.h"
#include "nsmiracle_aodv_packet_tracer.h"

MAODVPacketTracer::MAODVPacketTracer() :
    PacketTracer() {
}

MAODVPacketTracer::~MAODVPacketTracer() {
}

int MAODVPacketTracer::format(Packet *p, int offset, BaseTrace *pt,
        int newtrace) {
    hdr_cmn *ch = HDR_CMN(p);
    if (ch->ptype() == PT_MAODV) {
        format_aodvca_packet(p, offset, pt, newtrace);
        return 1;
    }

    return 0;
}

void MAODVPacketTracer::format_aodvca_packet(Packet *p, int offset,
        BaseTrace *pt, int newtrace) {
    struct hdr_maodv *hh = HDR_MAODV(p);
    struct hdr_maodv_request *rq = HDR_MAODV_REQUEST(p);
    struct hdr_maodv_reply *rp = HDR_MAODV_REPLY(p);

    /*
     switch (hh->aodvca_type) {
     case AODVCATYPE_PREQ:

     if (pt->tagged()) {
     sprintf(pt->buffer() + offset,
     "-aodvca:t %x -aodvca:h %d -aodvca:b %d -aodvca:d %d "
     "-aodvca:ds %d -aodvca:s %d -aodvca:ss %d "
     "-aodvca:c REQUEST ",
     rq->aodvca_type,
     rq->preq_hop_count,
     rq->preq_src_seqno,
     rq->preq_dst[0],
     rq->preq_dst_seqno[0],
     rq->preq_src,
     rq->preq_src_seqno);
     } else if (newtrace) {
     sprintf(pt->buffer() + offset,
     "-P aodvca -Pt 0x%x -Ph %d -Pb %d -Pd %d -Pds %d -Ps %d -Pss %d -Pc REQUEST ",
     rq->aodvca_type,
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
     case AODVCATYPE_PREP:
     if (pt->tagged()) {
     sprintf(pt->buffer() + offset,
     "-aodvca:t %x -aodvca:h %d -aodvca:d %d -adov:ds %d "
     "-aodvca:l %f -aodvca:c %s ",
     rp->aodvca_type,
     rp->prep_hop_count,
     rp->prep_dst,
     rp->prep_dst_seqno,
     rp->prep_lifetime,
     "PREP");
     } else if (newtrace) {
     sprintf(pt->buffer() + offset,
     "-P aodvca -Pt 0x%x -Ph %d -Pd %d -Pds %d -Pl %f -Pc %s ",
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
     "CMUTrace::format_aodvca: invalid AODVCA packet type\n");
     #else
     fprintf(stderr,
     "%s: invalid AODVCA packet type\n", __FUNCTION__);
     #endif
     abort();
     }*/
}

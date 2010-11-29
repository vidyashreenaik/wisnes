#ifndef __aodvrtt_packet_h__
#define __aodvrtt_packet_h__

#include <packet.h>

/* =====================================================================
 Packet Type...
 ===================================================================== */
extern packet_t PT_AODVRTT;

//#include <config.h>
//#include "aodvrtt.h"
#define AODVRTT_MAX_ERRORS 100

/* =====================================================================
 Packet Formats...
 ===================================================================== */
#define AODVRTTTYPE_HELLO    0x01
#define AODVRTTTYPE_RREQ     0x02
#define AODVRTTTYPE_RREP     0x04
#define AODVRTTTYPE_RERR     0x08
#define AODVRTTTYPE_RREP_ACK 0x10
#define AODVRTTTYPE_PROBE 	    0x12
#define AODVRTTTYPE_PROBE_ACK   0x14

#define AODVRTT_MAX_NEIGHBOURS 100

/*
 * AODVRTT Routing Protocol Header Macros
 */
#define HDR_AODVRTT(p)		        ((struct hdr_aodvrtt*)hdr_aodvrtt::access(p))
#define HDR_AODVRTT_REQUEST(p)      ((struct hdr_aodvrtt_request*)hdr_aodvrtt::access(p))
#define HDR_AODVRTT_REPLY(p)	    ((struct hdr_aodvrtt_reply*)hdr_aodvrtt::access(p))
#define HDR_AODVRTT_ERROR(p)	    ((struct hdr_aodvrtt_error*)hdr_aodvrtt::access(p))
#define HDR_AODVRTT_RREP_ACK(p)	    ((struct hdr_aodvrtt_rrep_ack*)hdr_aodvrtt::access(p))
#define HDR_AODVRTT_PROBE(p)	    ((struct hdr_aodvrtt_probe*)hdr_aodvrtt::access(p))
#define HDR_AODVRTT_PROBE_ACK(p)    ((struct hdr_aodvrtt_probe_ack*)hdr_aodvrtt::access(p))

/*
 * General AODVRTT Header - shared by all formats
 */
struct hdr_aodvrtt {
    u_int8_t ah_type;

    // Header access methods
    static int offset_; // required by PacketHeaderManager

    inline static int& offset() {
        return offset_;
    }

    inline static hdr_aodvrtt* access(const Packet* p) {
        return (hdr_aodvrtt*) p->access(offset_);
    }
};

struct hdr_aodvrtt_request {
    u_int8_t rq_type; // Packet Type
    u_int8_t reserved[2];
    u_int8_t rq_hop_count; // Hop Count
    double rq_rtt; // RTT
    u_int32_t rq_bcast_id; // Broadcast ID

    nsaddr_t rq_dst; // Destination IP Address
    u_int32_t rq_dst_seqno; // Destination Sequence Number
    nsaddr_t rq_src; // Source IP Address
    u_int32_t rq_src_seqno; // Source Sequence Number

    double rq_timestamp; // when REQUEST sent;
    // used to compute route discovery latency

    // This define turns on gratuitous replies- see aodvrtt.cc for implementation contributed by
    // Anant Utgikar, 09/16/02.
    // #define RREQ_GRAT_RREP	0x80

    inline int size() {
        int sz = sizeof(u_int8_t) // rq_type
                + 2 * sizeof(u_int8_t) // reserved
                + sizeof(u_int8_t) // rq_hop_count
                + sizeof(double) // rq_rtt
                + sizeof(u_int32_t) // rq_bcast_id
                + sizeof(nsaddr_t) // rq_dst
                + sizeof(u_int32_t) // rq_dst_seqno
                + sizeof(nsaddr_t) // rq_src
                + sizeof(u_int32_t) // rq_src_seqno
                + sizeof(u_int32_t); // rq_timestamp
        assert(sz >= 0);

        return sz;
    }
};

struct hdr_aodvrtt_reply {
    u_int8_t rp_type; // Packet Type
    u_int8_t reserved[2];
    u_int8_t rp_hop_count; // Hop Count
    double rp_rtt; // RTT
    nsaddr_t rp_dst; // Destination IP Address
    u_int32_t rp_dst_seqno; // Destination Sequence Number
    nsaddr_t rp_src; // Source IP Address
    double rp_lifetime; // Lifetime
    double rp_timestamp; // When corresponding REQ sent;
    // used to compute route discovery latency

    inline int size() {
        int sz = sizeof(u_int8_t) // rp_type
                + 2 * sizeof(u_int8_t) // reserved
                + sizeof(u_int8_t) // rp_hop_count
                + sizeof(double) // rp_rtt
                + sizeof(nsaddr_t) // rp_dst
                + sizeof(u_int32_t) // rp_dst_seqno
                + sizeof(nsaddr_t) // rp_src
                + sizeof(double) // rp_lifetime
                + sizeof(double); // rp_timestamp
        assert(sz >= 0);

        return sz;
    }
};

struct hdr_aodvrtt_probe {
    u_int8_t rb_type; // Packet Type
    nsaddr_t rb_src; // Source IP Address
    u_int32_t rb_bcast_id; // Broadcast ID
    double rb_timestamp; // When packet was sent

    inline int size() {
        int sz = sizeof(u_int8_t) // rp_type
                + sizeof(nsaddr_t) // rb_src
                + sizeof(u_int32_t) // rb_bcast_id
                + sizeof(double); // rb_timestamp
        assert(sz >= 0);

        return sz;
    }
};

struct hdr_aodvrtt_probe_ack {
    u_int8_t pa_type; // Packet Type
    nsaddr_t pa_src; // Source IP Address
    nsaddr_t pa_dst; // Destination IP Address
    double pa_timestamp; // When packet was sent

    inline int size() {
        int sz = sizeof(u_int8_t) // pa_type
                + sizeof(nsaddr_t) // pa_src
                + sizeof(nsaddr_t) // pa_dst
                + sizeof(double); // pa_timestamp
        assert(sz >= 0);

        return sz;
    }
};

struct hdr_aodvrtt_error {
    u_int8_t re_type; // Type
    u_int8_t reserved[2]; // Reserved
    u_int8_t DestCount; // DestCount
    // List of Unreachable destination IP addresses and sequence numbers
    nsaddr_t unreachable_dst[AODVRTT_MAX_ERRORS];
    u_int32_t unreachable_dst_seqno[AODVRTT_MAX_ERRORS];

    inline int size() {
        int sz = sizeof(u_int8_t) // re_type
                + 2 * sizeof(u_int8_t) // reserved
                + sizeof(u_int8_t) // DestCount
                + DestCount * sizeof(nsaddr_t) // unreachable destinations
                + DestCount * sizeof(u_int32_t); // sequence numbers
        assert(sz);

        return sz;
    }
};

struct hdr_aodvrtt_rrep_ack {
    u_int8_t rpack_type;
    u_int8_t reserved;
};

// for size calculation of header-space reservation
union hdr_all_aodvrtt {
    hdr_aodvrtt ah;
    hdr_aodvrtt_request rreq;
    hdr_aodvrtt_reply rrep;
    hdr_aodvrtt_error rerr;
    hdr_aodvrtt_rrep_ack rrep_ack;
    hdr_aodvrtt_probe rprob;
    hdr_aodvrtt_probe_ack rprob_ack;
};

#endif /* __aodvrtt_packet_h__ */

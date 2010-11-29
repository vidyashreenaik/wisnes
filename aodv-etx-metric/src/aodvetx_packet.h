/*
 Copyright (c) 1997, 1998 Carnegie Mellon University.  All Rights
 Reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:

 1. Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.
 2. Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.
 3. The name of the author may not be used to endorse or promote products
 derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 The AODVETX code developed by the CMU/MONARCH group was optimized and tuned by Samir Das and Mahesh Marina, University of Cincinnati. The work was partially done in Sun Microsystems.
 */

#ifndef __aodvetx_packet_h__
#define __aodvetx_packet_h__

#include <packet.h>

/* =====================================================================
 Packet Type...
 ===================================================================== */
extern packet_t PT_AODVETX;

//#include <config.h>
//#include "aodvetx.h"
#define AODVETX_MAX_ERRORS 100

/* =====================================================================
 Packet Formats...
 ===================================================================== */
#define AODVETXTYPE_HELLO    0x01
#define AODVETXTYPE_RREQ     0x02
#define AODVETXTYPE_RREP     0x04
#define AODVETXTYPE_RERR     0x08
#define AODVETXTYPE_RREP_ACK 0x10
#define AODVETXTYPE_PROBE 	 0x12

#define AODVETX_MAX_NEIGHBOURS 100

/*
 * AODVETX Routing Protocol Header Macros
 */
#define HDR_AODVETX(p)		    ((struct hdr_aodvetx*)hdr_aodvetx::access(p))
#define HDR_AODVETX_REQUEST(p)  ((struct hdr_aodvetx_request*)hdr_aodvetx::access(p))
#define HDR_AODVETX_REPLY(p)	((struct hdr_aodvetx_reply*)hdr_aodvetx::access(p))
#define HDR_AODVETX_ERROR(p)	((struct hdr_aodvetx_error*)hdr_aodvetx::access(p))
#define HDR_AODVETX_RREP_ACK(p)	((struct hdr_aodvetx_rrep_ack*)hdr_aodvetx::access(p))
#define HDR_AODVETX_PROBE(p)	((struct hdr_aodvetx_probe*)hdr_aodvetx::access(p))

/*
 * General AODVETX Header - shared by all formats
 */
struct hdr_aodvetx {
    u_int8_t ah_type;
    /*
     u_int8_t        ah_reserved[2];
     u_int8_t        ah_hopcount;
     */
    // Header access methods
    static int offset_; // required by PacketHeaderManager

    inline static int& offset() {
        return offset_;
    }

    inline static hdr_aodvetx* access(const Packet* p) {
        return (hdr_aodvetx*) p->access(offset_);
    }
};

struct hdr_aodvetx_request {
    u_int8_t rq_type; // Packet Type
    u_int8_t reserved[2];
    u_int8_t rq_hop_count; // Hop Count
    double rq_etx; // ETX
    u_int32_t rq_bcast_id; // Broadcast ID

    nsaddr_t rq_dst; // Destination IP Address
    u_int32_t rq_dst_seqno; // Destination Sequence Number
    nsaddr_t rq_src; // Source IP Address
    u_int32_t rq_src_seqno; // Source Sequence Number

    double rq_timestamp; // when REQUEST sent;
    // used to compute route discovery latency

    // This define turns on gratuitous replies- see aodvetx.cc for implementation contributed by
    // Anant Utgikar, 09/16/02.
    //#define RREQ_GRAT_RREP	0x80

    inline int size() {
        int sz = sizeof(u_int8_t) // rq_type
                + 2 * sizeof(u_int8_t) // reserved
                + sizeof(u_int8_t) // rq_hop_count
                + sizeof(double) // rq_etx
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

struct hdr_aodvetx_reply {
    u_int8_t rp_type; // Packet Type
    u_int8_t reserved[2];
    u_int8_t rp_hop_count; // Hop Count
    double rp_etx; // ETX
    nsaddr_t rp_dst; // Destination IP Address
    u_int32_t rp_dst_seqno; // Destination Sequence Number
    nsaddr_t rp_src; // Source IP Address
    double rp_lifetime; // Lifetime
    double rp_timestamp; // when corresponding REQ sent;
    // used to compute route discovery latency

    inline int size() {
        int sz = sizeof(u_int8_t) // rp_type
                + 2 * sizeof(u_int8_t) // reserved
                + sizeof(u_int8_t) // rp_hop_count
                + sizeof(double) // ETX
                + sizeof(nsaddr_t) // rp_dst
                + sizeof(u_int32_t) // rp_dst_seqno
                + sizeof(nsaddr_t) // rp_src
                + sizeof(double) // rp_lifetime
                + sizeof(double); // rp_timestamp
        assert(sz >= 0);

        return sz;
    }
};

struct hdr_aodvetx_probe {
    u_int8_t rb_type; // Packet Type
    nsaddr_t rb_src; // Source IP Address
    u_int32_t rb_bcast_id; // Broadcast ID
    u_int8_t rb_neighbour_count; // Number of neighbours from which probes have been received
    nsaddr_t rb_neighbours[AODVETX_MAX_NEIGHBOURS]; // Their addresses
    u_int8_t rb_probes[AODVETX_MAX_NEIGHBOURS]; // Number of the probes received
    double rb_timestamp; // when it was sent

    inline int size() {
        int sz = sizeof(u_int8_t) // rp_type
                + sizeof(nsaddr_t) // rb_src
                + sizeof(u_int32_t) // rb_bcast_id
                + sizeof(u_int8_t) // rb_neighbour_count
                + (rb_neighbour_count) * sizeof(nsaddr_t) // neighbours
                + (rb_neighbour_count) * sizeof(u_int8_t) // probes
                + sizeof(double); // rb_timestamp
        assert(sz >= 0);

        return sz;
    }
};

struct hdr_aodvetx_error {
    u_int8_t re_type; // Type
    u_int8_t reserved[2]; // Reserved
    u_int8_t DestCount; // DestCount
    // List of Unreachable destination IP addresses and sequence numbers
    nsaddr_t unreachable_dst[AODVETX_MAX_ERRORS];
    u_int32_t unreachable_dst_seqno[AODVETX_MAX_ERRORS];

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

struct hdr_aodvetx_rrep_ack {
    u_int8_t rpack_type;
    u_int8_t reserved;
};

// for size calculation of header-space reservation
union hdr_all_aodvetx {
    hdr_aodvetx ah;
    hdr_aodvetx_request rreq;
    hdr_aodvetx_reply rrep;
    hdr_aodvetx_error rerr;
    hdr_aodvetx_rrep_ack rrep_ack;
    hdr_aodvetx_probe rprob;
};

#endif /* __aodvetx_packet_h__ */

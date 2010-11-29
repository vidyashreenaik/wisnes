/*
 * hwmp_packet.h
 * Copyright (C) 2008 by the Institute for Information Transmission Problems
 * Originally written by Kirill V. Andreev <kirillano@yandex.ru> and Andrey I. Mazo <ahippo@yandex.ru>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 *
 *
 * The copyright of this module includes the following
 * linking-with-specific-other-licenses addition:
 *
 * In addition, as a special exception, the copyright holders of
 * this module give you permission to combine (via static or
 * dynamic linking) this module with free software programs or
 * libraries that are released under the GNU LGPL and with code
 * included in the standard release of ns-2 under the Apache 2.0
 * license or under otherwise-compatible licenses with advertising
 * requirements (or modified versions of such code, with unchanged
 * license).  You may copy and distribute such a system following the
 * terms of the GNU GPL for this module and the licenses of the
 * other code concerned, provided that you include the source code of
 * that other code when and as the GNU GPL requires distribution of
 * source code.
 *
 * Note that people who make modified versions of this module
 * are not obligated to grant this special exception for their
 * modified versions; it is their choice whether to do so.  The GNU
 * General Public License gives permission to release a modified
 * version without this exception; this exception also makes it
 * possible to release a modified version which carries forward this
 * exception.
 *
 */

/*
 * $Id: hwmp_packet.h 44 2008-08-04 22:34:52Z hippo $
 */



#ifndef __HWMP_packet_h__
#define __HWMP_packet_h__

#include <packet.h>

/* =====================================================================
   Packet Type...
   ===================================================================== */
extern packet_t PT_HWMP;

/* =====================================================================
   Packet Formats...
   ===================================================================== */
#define HWMPTYPE_PREQ		0x02
#define HWMPTYPE_PREP		0x04
#define HWMPTYPE_PERR		0x08

/*
 * HWMP Routing Protocol Header Macros
 */
#define HDR_HWMP(p)		((struct hdr_hwmp*)hdr_hwmp::access(p))
#define HDR_HWMP_PREQ(p)  	((struct hdr_hwmp_preq*)hdr_hwmp::access(p))
#define HDR_HWMP_PREP(p)	((struct hdr_hwmp_prep*)hdr_hwmp::access(p))
#define HDR_HWMP_PERR(p)	((struct hdr_hwmp_perr*)hdr_hwmp::access(p))

/*
 * General HWMP Header - shared by all formats
 */
struct hdr_hwmp {
	u_int8_t	hwmp_type;
	u_int8_t	hwmp_ttl;
	static int	offset_; // required by PacketHeaderManager

	inline static int& offset() { return offset_; }
	inline static hdr_hwmp* access(const Packet* p) {
		return (hdr_hwmp*) p->access(offset_);
	}
};

struct hdr_hwmp_preq {
	u_int8_t	hwmp_type;	// from hdr_hwmp
	u_int8_t	hwmp_ttl;	// from hdr_hwmp

	u_int8_t	preq_type;	// Packet Type - only for NS
#define MAX_RT_TTL	0xff
	u_int8_t	preq_flags;
#define PREQ_F_PORTAL		0x1	// is set if we are a portal
#define PREQ_F_GROUP		0x2	// is set if group address in the frame
#define PREQ_F_PROACTIVE	0x4	// is set when sending proactive preq
#define PREQ_F_AE		0x40	// Address extension
	// See 7.3.2.70 of 802.11s Draft
	double		preq_timestamp;	// when PREQ was sent: for NS and for
					// lifetime in routing table
	u_int8_t	preq_hop_count;	// Hop Count
	u_int32_t	preq_id;	// ID of PREQ
	u_int32_t	preq_src;	// Source MAC
	u_int32_t	preq_src_seqno;	// Source Sequence Number
	u_int32_t	preq_proxyaddr;	// Proxied address
	double		preq_lifetime;	// The time of sending PREQ --
					// stored in rtable as time,
					// when routing info is valid
#define PREQ_LIFETIME		30
#define PREQ_PROACT_LIFETIME	20
	double		preq_airtime;			// Airtime metric
#define PREQ_MAX_DSTS		10
	u_int8_t	preq_dst_count;			// Destination count
	u_int32_t	preq_dst[PREQ_MAX_DSTS];	// Destination MAC
	u_int32_t	preq_dst_seqno[PREQ_MAX_DSTS];	// Destination Sequence Number
#define PREQ_DO		0x01
#define PREQ_RF		0x02
#define PREQ_PROACTIVE	(PREQ_DO | PREQ_RF)
	u_int8_t	preq_dst_flags[PREQ_MAX_DSTS];	// Destination flags

	inline int size() {
		int sz = 0;
		sz =	  sizeof(u_int8_t)			// hwmp_type
			+ sizeof(u_int8_t)			// hwmp_ttl
			+ sizeof(u_int8_t)			// preq_type
			+ sizeof(u_int8_t)			// preq_flags
			+ sizeof(double)			// preq_timestamp
			+ sizeof(u_int8_t)			// preq_hop_count
			+ sizeof(u_int32_t)			// preq_id
			+ sizeof(u_int32_t)			// preq_src
			+ sizeof(u_int32_t)			// preq_src_seqno
			+ sizeof(u_int32_t)			// preq_proxyaddr
			+ sizeof(double)			// preq_lifetime
			+ sizeof(double)			// preq_airtime
			+ sizeof(u_int8_t)			// preq_dstcount
			+ sizeof(u_int32_t) * PREQ_MAX_DSTS	// preq_dst[PREQ_MAX_DSTS]
			+ sizeof(u_int32_t) * PREQ_MAX_DSTS	// preq_dst_seqno[PREQ_MAX_DSTS]
			+ sizeof(u_int8_t)  * PREQ_MAX_DSTS;	// preq_dst_flags[PREQ_MAX_DSTS]

		assert (sz >= 0);
		return sz;
	}
};

struct hdr_hwmp_prep {
	u_int8_t	hwmp_type;	// from hdr_hwmp
	u_int8_t	hwmp_ttl;	// from hdr_hwmp

	u_int8_t	prep_type;	// Packet type - for NS
	u_int32_t	prep_id;
	u_int8_t	prep_flags;	// prep flags:
#define PREP_F_AE		0x40	// Address extension
	// See 7.3.2.71 of 802.11s Draft
	double		prep_timestamp;	//when PREP was sent - only for NS
	u_int8_t	prep_hop_count;
	u_int32_t	prep_dst;
	u_int32_t	prep_dst_seqno;
	u_int32_t	prep_proxyaddr;
	double		prep_lifetime;
	double		prep_airtime;
	u_int32_t	prep_src;
	u_int32_t	prep_src_seqno;
#define PREP_MAX_DEPS		PREQ_MAX_DSTS
	u_int8_t	prep_dep_count;
	u_int32_t	prep_dep[PREP_MAX_DEPS];
	u_int32_t	prep_dep_seqno[PREP_MAX_DEPS];

	inline int size() {
		int sz = 0;
		sz =	  sizeof(u_int8_t)			// hwmp_type
			+ sizeof(u_int8_t)			// hwmp_ttl
			+ sizeof(u_int8_t)			// prep_type
			+ sizeof(u_int32_t)			// prep_id
			+ sizeof(u_int8_t)			// prep_flags
			+ sizeof(double)			// prep_timestamp
			+ sizeof(u_int8_t)			// prep_hop_count
			+ sizeof(u_int32_t)			// prep_dst
			+ sizeof(u_int32_t)			// prep_dst_seqno
			+ sizeof(u_int32_t)			// prep_proxyaddr
			+ sizeof(double)			// prep_lifetime
			+ sizeof(double)			// prep_airtime
			+ sizeof(u_int32_t)			// prep_src
			+ sizeof(u_int32_t)			// prep_src_seqno
			+ sizeof(u_int8_t)			// prep_dep_count
			+ sizeof(u_int32_t) * PREP_MAX_DEPS	// prep_dep[PREP_MAX_DEPS]
			+ sizeof(u_int32_t) * PREP_MAX_DEPS;	// prep_dep_seqno[PREP_MAX_DEPS]

		assert (sz >= 0);
		return sz;
	}
};


struct hdr_hwmp_perr {
	u_int8_t	hwmp_type;			// from hdr_hwmp
	u_int8_t	hwmp_ttl;			// from hdr_hwmp

	u_int8_t	perr_type;			// Packet type - for NS
	u_int32_t	perr_id;
	u_int8_t	perr_flags;			// prep flags

	// See 7.3.2.72 of 802.11s Draft
#define PERR_MAX_DSTS		PREQ_MAX_DSTS
	u_int8_t	perr_dst_count;			// Destination count
	// List of Unreachable destination IP addresses and sequence numbers
	u_int32_t	perr_dst[PERR_MAX_DSTS];
	u_int32_t	perr_dst_seqno[PERR_MAX_DSTS];

	inline int size() {
		int sz = 0;
		sz =	  sizeof(u_int8_t)			// hwmp_type
			+ sizeof(u_int8_t)			// hwmp_type
			+ sizeof(u_int8_t)			// hwmp_ttl
			+ sizeof(u_int8_t)			// perr_type
			+ sizeof(u_int32_t)			// perr_id
			+ sizeof(u_int8_t)			// perr_flags
			+ sizeof(u_int8_t)			// perr_dst_count
			+ sizeof(u_int32_t) * PERR_MAX_DSTS	// perr_dst[PERR_MAX_DSTS]
			+ sizeof(u_int32_t) * PERR_MAX_DSTS;	// perr_dst_seqno[PERR_MAX_DSTS]

		assert (sz >= 0);
		return sz;
	}
};

// for size calculation of header-space reservation
union hdr_all_hwmp
{
	hdr_hwmp	hwmp;
	hdr_hwmp_preq	preq;
	hdr_hwmp_prep	prep;
	hdr_hwmp_perr	perr;
};
#endif /* __HWMP_packet_h__ */

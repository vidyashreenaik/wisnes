/*
 * hwmp_rtable.h
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
 * $Id: hwmp_rtable.h 44 2008-08-04 22:34:52Z hippo $
 */



#ifndef __hwmp_rtable_h__
#define __hwmp_rtable_h__

#include <assert.h>
#include <sys/types.h>
#include <config.h>
#include <lib/bsd-list.h>
#include <scheduler.h>
#include <mac/mac.h>

#define CURRENT_TIME	Scheduler::instance().clock()

class hwmp_rt_entry {
	friend class hwmp_rtable;
	friend class HWMP;

	public:
		hwmp_rt_entry();		// Constructor

	protected:
		// Destination parameters:
		u_int32_t	rt_dst;		// DST mac
		u_int32_t	rt_dst_seqno;	// Destination Sequence Number
		u_int32_t	rt_dst_preqID;	// Destination PREQ ID
		// Routing parameters:
		u_int16_t	rt_hop_count;	// Hops to the destination
		double		rt_airtime;	// Airtime metric for HWMP
#define AIR_OVERHEAD		7.812
#define AIR_MULTIPLIER		1000.0
#define MAX_METRIC		100000.0
		// We suppose now that we have no errors during retransmitting, the
		// rate is constant and equals to 1 MBit/sec.
		// And the shadowing model is turned OFF
		// So, the final formula is
		// metric += traveling_time*AIR_MULTIPLIER + AIR_OVERHEAD
		u_int32_t	rt_prev_hop;	// Previous hop - used to send a PERR
		u_int32_t	rt_next_hop;	// Next hop -- where to send a packet
		double		rt_lifetime;	// The last moment of usability this routing info
#define RT_LIFETIME		10		// seconds
		u_int8_t	rt_retry;	// The number of retryes to establish path
		double		rt_retry_thresh;// Time when retry needed
#define MAX_RETRY		10
		u_int8_t	rt_flags;
#define RTF_INIT		0	// Initial state of rtable entry
#define RTF_NOPREP		1	// The PREQ was sent, waiting for PREP
#define RTF_ACTUAL		2	// The PREP was received and the table is actual
#define RTF_RETRY		3	// If no PREP was received and retry timer is exipred
#define RTF_FAILED		4	// If the number of retryes expired
#define RTF_OLD			5	// If the lifetime is expired
		u_int8_t	rt_to_root; 
#define NOT_TO_ROOT		0
#define IS_TO_ROOT		1
		LIST_ENTRY(hwmp_rt_entry)	rt_link;	// List of route entries
};

class hwmp_rtable {
	friend class HWMP;
	public:
		hwmp_rtable() {
			LIST_INIT(&rthead);
			rtable_preq_limit = CURRENT_TIME;
		}

		hwmp_rt_entry*		head() { return rthead.lh_first; }
		// Obtain a routing table instance:
		hwmp_rt_entry*		rtable_lookup(u_int32_t dst);
		hwmp_rt_entry*		rtable_add(u_int32_t adr);
		void			rtable_del(u_int32_t dst);
		double			rtable_get_preqdelay(u_int8_t retry);

	protected:
		LIST_HEAD(hwmp_rthead, hwmp_rt_entry)	rthead;
		//The time until which we are not able to send PREQ:
		double 			rtable_preq_limit;
#define PREQ_DISTANCE		0.01	// seconds
		// This constant is used to calculate exponetial backoff:
		// Timeslot is the start value of backoff
#define PREQ_TIMESLOT		0.01	// seconds
#define PREQ_RETRY		0.1
#define CONFIRM_INTERVAL	10	// seconds
#define PROACTIVE_TIMESLOT	10	// seconds
};
#endif


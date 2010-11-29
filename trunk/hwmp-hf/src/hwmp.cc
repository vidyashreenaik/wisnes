/*
 * hwmp.cc
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
 * $Id: hwmp.cc 183 2009-01-16 00:16:27Z hippo $
 */

//#define ROOT_INDEX (4)


#include "hwmp.h"
#include "hwmp_packet.h"
#include <random.h>
#include <cmu-trace.h>
#include <mobilenode.h>

/*
   TCL Hooks
 */

int hdr_hwmp::offset_;

static class HWMPHeaderClass : public PacketHeaderClass {
	public:
		HWMPHeaderClass() : PacketHeaderClass("PacketHeader/HWMP",
				sizeof(hdr_all_hwmp)) {
			bind_offset(&hdr_hwmp::offset_);
			bind();
		}
} class_rtProtoHWMP_hdr;

static class HWMPclass : public TclClass {
	public:
		HWMPclass() : TclClass("Agent/HWMP") {}
		TclObject* create(int argc, const char*const* argv) {
			assert(argc == 5);

			return (new HWMP((nsaddr_t) Address::instance().str2addr(argv[4])));
		}
} class_rtProtoHWMP;


int
HWMP::command(int argc, const char*const* argv) {
	if(argc == 2) {
		Tcl& tcl = Tcl::instance();
		if(strncasecmp(argv[1], "id", 2) == 0) {
			tcl.resultf("%d", index);
			return TCL_OK;
		}
		if(strncasecmp(argv[1], "start", 2) == 0) {
			//TODO: PerLink timers must be here
#ifdef ROOT_INDEX
			proactivePREQ_timer.handle((Event*) 0);
#endif
			return TCL_OK;
		}
	}
	else if(argc == 3) {
		if(strcmp(argv[1], "index") == 0) {
			index_ = atoi(argv[2]);
			return TCL_OK;
		} else  if (strcmp(argv[1], "addr") == 0) {
            index_ =  Address::instance().str2addr(argv[2]);
            return TCL_OK;
        } else if(strcmp(argv[1], "log-target") == 0 || strcmp(argv[1], "tracetarget") == 0) {
			logtarget = (Trace*) TclObject::lookup(argv[2]);
			if(logtarget == 0)
				return TCL_ERROR;
			return TCL_OK;
		}
		else if(strcmp(argv[1], "drop-target") == 0) {
			int stat = rqueue.command(argc,argv);
			if (stat != TCL_OK) return stat;
			return Agent::command(argc, argv);
		}
		else if(strcmp(argv[1], "if-queue") == 0) {
			ifqueue = (PriQueue*) TclObject::lookup(argv[2]);
			if(ifqueue == 0)
				return TCL_ERROR;
			return TCL_OK;
		} else if (strcasecmp (argv[1], "node") == 0) {
	          node_ = (MobileNode*) TclObject::lookup(argv[2]);
	          return TCL_OK;
        } else if (strcmp(argv[1], "port-dmux") == 0) {
			dmux_ = (PortClassifier *)TclObject::lookup(argv[2]);
			if (dmux_ == 0) {
				fprintf(
						stderr,
						"%s: %s lookup of %s failed\n",
						__FILE__,
						argv[1],
						argv[2]);
				return TCL_ERROR;
			}
			return TCL_OK;
		}
	}
	return Agent::command(argc, argv);
}


void
HWMP::recv(Packet *p, Handler*) {
	struct hdr_cmn *ch		= HDR_CMN(p);
	struct hdr_ip *ip		= HDR_IP(p);

	assert(initialized());
	if(ch->ptype() == PT_HWMP)
	{
		recvHWMP(p);
		return;
	}
    
	/*
	 * fill last_hop_ with IP destination address.
	 * MAC destination address should be here, but we can't use ARP here, so
	 * we just pust IP address here (as it's the same with MAC address).
	 * This is ugly but works.:)
	 */
	//ch->last_hop_ = ip->daddr();
    // Modification: Fill last_hop_ with base station address if it is towards
    // wired node
    if (HWMP::diff_subnet(ip->daddr(), index_))
    {
        HDPRINTF("Inter-Domain Packet: D = %d | Redirecting: B = %d", ip->daddr(), node_->base_stn());
        ch->last_hop_ = (u_int32_t) node_->base_stn();
    }
    else
    {
        ch->last_hop_ = ip->daddr();
    }
	//If we are the source of the packet, fill it's timestamp
	if(ip->saddr() == index_)
    {
        ch->ts_ = CURRENT_TIME;
    }
	if(ch->num_forwards() > 3)
	{
		Packet::free(p);
		return;
	}
	ch->num_forwards() ++;
	if (((u_int32_t)(ch->last_hop_)) != MAC_BROADCAST)
		rt_resolve(p);
	else
		forward(NULL, p);
}

void
HWMP::rt_resolve(Packet* p)
{
	struct hdr_cmn	*ch = HDR_CMN(p);
    struct hdr_ip *ip	= HDR_IP(p);
	hwmp_rt_entry	*rt;
    
    if ((((u_int32_t) node_->base_stn()) == index_) && HWMP::diff_subnet(ip->daddr(), index_))// && (ch->last_hop_ == index_))
    {
        // Base-Station, deliver to wired
        HDPRINTF("Inter-Domain Packet: D = %d | Base station: Delivering to wired", ip->daddr());
        ch->last_hop_ = ip->daddr();    
        ch->direction()	= hdr_cmn::UP;
        ch->addr_type() = NS_AF_ILINK;
        
        dmux_->recv(p, 0);
        return;
    }

	/*
	 * 1. Looking for reactive path
	 */
	rt = rtable.rtable_lookup(ch->last_hop_);
	if(rt != NULL)
	{
		if(rt->rt_flags == RTF_ACTUAL)
		{
			if(rt->rt_lifetime > CURRENT_TIME)
			{
                HDPRINTF("VALID path to %d: Forwarding", ch->last_hop_);
				// forwarding information is still active
				forward(rt, p);
				return;
			}
			else 
				rt->rt_flags = RTF_OLD; // mark forwarding information old
		}
	}
	/*
	 * 2. Looking for proactive path
	 */
	if(mode == PROACTIVE)
	{
		rt = rtable.rtable_lookup(root_node);
		if((rt != NULL) && (rt->rt_lifetime > CURRENT_TIME))
		{
			forward(rt, p);
			return;
		}
	}
	/*
	 * 3. Sending PREQ reactively
	 */
    HDPRINTF("INVALID path to %d: Sending PREQ reactively", ch->last_hop_);
	rt = rtable.rtable_add(ch->last_hop_);
	sendPREQ(ch->last_hop_);
	rqueue.enque(p);
}

void
HWMP::recvHWMP(Packet *p) {
	struct hdr_hwmp *hh = HDR_HWMP(p);

	hh->hwmp_ttl --;
	if(hh->hwmp_ttl == 0)
	{
		Packet::free(p);
		return;
	}
	switch(hh->hwmp_type) {
		case HWMPTYPE_PREQ:
			recvPREQ(p);
			break;
		case HWMPTYPE_PREP:
			recvPREP(p);
			break;
		case HWMPTYPE_PERR:
			//recvPERR(p);
			break;
		default:
			return;
	}
}


HWMP::HWMP(u_int32_t mac) : Agent(PT_HWMP), proactivePREQ_timer(this), rqueue()
{
	index_		= mac;
	root_node	= -1;

	seqno		= 1;
	preq_id		= 0;
	prep_id		= 0;
	logtarget	= 0;
	ifqueue		= 0;
	mode		= REACTIVE;
}

void
HWMP::forward(hwmp_rt_entry* rt, Packet* p)
{
    struct hdr_cmn *ch = HDR_CMN(p);
    
    if ((ch->ptype() != PT_HWMP)
            && (ch->direction() == hdr_cmn::UP)
            && ((((u_int32_t) (ch->last_hop_)) == index_) || (((u_int32_t) (ch->last_hop_)) == MAC_BROADCAST))) {
        // Deliver packet to upper layers
        HDPRINTF("Delivering to upper layers");
        dmux_->recv(p, 0);
        return;
    }
    if (rt != NULL) {
        ch->next_hop_ = rt->rt_next_hop;
        ch->error() = 0;
        ch->direction() = hdr_cmn::DOWN;
        ch->addr_type() = NS_AF_INET;
    } else {
        // Forwarding a broadcast frame!
        ch->direction() = hdr_cmn::DOWN;
        ch->addr_type() = NS_AF_INET;
    }
#ifdef MYTRACE
    printf("%d\t%d\n", index_, ch->uid_);
#endif
    Scheduler::instance().schedule(target_, p, 0.);
}


void
HWMP::sendPREQ(u_int32_t dst) {
	Packet			*p	= Packet::alloc();
	struct hdr_cmn		*ch	= HDR_CMN(p);
	struct hdr_hwmp_preq	*rq	= HDR_HWMP_PREQ(p);
	struct hdr_ip		*ip	= HDR_IP(p);
	struct hdr_hwmp		*hh	= HDR_HWMP(p);
	hwmp_rt_entry		*rt	= rtable.rtable_lookup(dst);
	short			proactive = 0;
	/*
	 * 802.11s 11A.8.5.2  Conditions for generating and sending a PREQ
	 * Cases A,B,C,E
	 */

	HDPRINTF("started! dst = %d, metric = %2.4f, %d",
			dst,
			rq->preq_airtime,
			rq->preq_hop_count);
	if (dst == MAC_BROADCAST)
	{
		HDPRINTF("Proactive PREQ started! metric = %2.4f, %d",
				rq->preq_airtime,
				rq->preq_hop_count);
		//TODO: set root MP from tcl script!
#ifdef ROOT_INDEX
		if(index_ != ROOT_INDEX)
		{
			Packet::free(p);
			return;
		}
		mode = PROACTIVE;
		root_node = index_;
		proactive = 1;
#else
		Packet::free(p);
		return;
#endif
	}
	// The routing table entry must be added before we start route discovery procedure
	if((rt == NULL) && (!proactive)) {
		Packet::free(p);
		return;
	}
	if(!proactive) {
		// Check timeout for this request
		if((rt->rt_flags == RTF_NOPREP) || (rt->rt_flags == RTF_RETRY))
		{
			HDPRINTF("PREQ was already sent!");

			if(rt->rt_retry_thresh < CURRENT_TIME)
			{
				rt->rt_flags = RTF_RETRY;
				rt->rt_retry ++;
				rt->rt_retry_thresh = CURRENT_TIME + PREQ_RETRY + rt->rt_retry*PREQ_TIMESLOT;
				// TODO: must be exponential backoff;
			}
			else
			{
				HDPRINTF("No retry flag! DELETE PREQ!");

				Packet::free(p);
				return;
			}
		}
		if((rt->rt_flags == RTF_INIT) || (rt->rt_flags == RTF_OLD))
		{
			rt->rt_flags = RTF_NOPREP;
			rt->rt_retry_thresh = CURRENT_TIME + PREQ_RETRY;
		}

		/*
		 * 802.11s 11A.8.5.2  Conditions for generating and sending a PREQ
		 * Cases A,B,C
		 */
		// TODO: check timeouts
		if (rt->rt_lifetime < CURRENT_TIME)
			rt->rt_flags &= ~RTF_ACTUAL;

		if (rt->rt_flags & RTF_ACTUAL)
		{
			Packet::free(p);
			return;
		}
		//TODO: retry + PERR
		if(rt->rt_flags & RTF_FAILED)
		{
			/* Previous path discovery has failed.
			 * There is at least 3 options here:
			 * 1) Drop the packet
			 * 2) Start a timer, wait for it to expire, and after
			 *    that send a new request if needed
			 * 3) Restart path discovery at a next data packet
			 * For now we choose 1).
			 * TODO: let user choose and set from tcl script
			 * TODO: all packets must be dropped here from the queue
			 */
			Packet::free(p);
			return;
		}
	}
	/*
	 * Filling the packet fields:
	 */
	// 1. Common header:
	ch->size()		= rq->size();
	ch->iface()		= -2;
	ch->error()		= 0;
	ch->addr_type()		= NS_AF_INET;
	ch->prev_hop_		= index_;
	ip->sport()		= RT_PORT;
	ip->dport()		= RT_PORT;
	ch->next_hop_		= MAC_BROADCAST;
	ch->last_hop_		= dst;
	ch->ptype()		= PT_HWMP;
	ch->direction()		= hdr_cmn::DOWN;
	ch->num_forwards()	= 0;
	rq->preq_timestamp	= CURRENT_TIME;

	// 2. PREQ header:
	// length = 27 + 11*N // unneeded in NS
	hh->hwmp_type		= HWMPTYPE_PREQ;
	// Flags: Broadcast
	rq->preq_flags		= 0;
	if(proactive)
		rq->preq_flags	|= PREQ_F_PROACTIVE;
	rq->preq_hop_count	= 0;
	hh->hwmp_ttl		= MAX_RT_TTL;
	preq_id ++;
	rq->preq_id		= preq_id;
	rq->preq_src		= index_;
	seqno ++;
	rq->preq_src_seqno	= seqno;
	//Proxied Address //unneeded for our simulation
	if(proactive)
		rq->preq_lifetime	= CURRENT_TIME + PREQ_PROACT_LIFETIME;
	else
		rq->preq_lifetime	= CURRENT_TIME + PREQ_LIFETIME;
	rq->preq_airtime	= 0;
	// destination count is only 1 now, so unused
	// TODO: set DO,RF flags from tcl script
	// Case A: DO = 0, RF = 0;
	rq->preq_dst_flags[0]	= 0;
	//rq->preq_dst_flags[0]	|= PREQ_DO; //uncomment this to set DO flag globally
	//rq->preq_dst_flags[0]	|= PREQ_RF; //uncomment this to set RF flag globally
	if(proactive)
		rq->preq_dst_flags[0]	|= (PREQ_DO | PREQ_RF);
	rq->preq_dst[0]		= dst;
	rq->preq_dst_seqno[0]	= proactive?seqno:(rt->rt_dst_seqno);
	if(!proactive){
#ifdef MYTRACE
		printf("PREQ:index:%d\n", index_);
#endif
		if(rtable.rtable_preq_limit < CURRENT_TIME)
			rtable.rtable_preq_limit = CURRENT_TIME;
		else
			rq->preq_timestamp -= (rtable.rtable_preq_limit-CURRENT_TIME);
		Scheduler::instance().schedule(target_, p, rtable.rtable_preq_limit-CURRENT_TIME);
		rtable.rtable_preq_limit += PREQ_DISTANCE;
	}
	else {
		Scheduler::instance().schedule(target_, p, 0.0);
	}
}

void
HWMP::recvPREQ (Packet *p)
{
	struct hdr_hwmp_preq	*preq		= HDR_HWMP_PREQ(p);
	struct hdr_cmn		*ch		= HDR_CMN(p);
	hwmp_rt_entry		*rtback, *rtforw, *rtnext;

	if(preq->preq_src == index_)
	{
		// Needed to avoid "path to me is throw perlink"
		Packet::free(p);
		return;
	}
	rtback = rtable.rtable_lookup(preq->preq_src);
	if(rtback == NULL)
		rtback = rtable.rtable_add(preq->preq_src);
	preq->preq_hop_count ++;
	preq->preq_airtime	+= (CURRENT_TIME - preq->preq_timestamp) * AIR_MULTIPLIER
					+ AIR_OVERHEAD;
	/*
	 * 802.11s 11A.8.5.3.1 Acceptance criteria:
	 */
	HDPRINTF("started! dst = %d, metric = %2.4f, %d",
			preq->preq_dst[0],
			preq->preq_airtime,
			preq->preq_hop_count);
	//drop old PREQs and PREQs with bad airtime metric
	if((preq->preq_src_seqno < rtback->rt_dst_seqno)
			|| ((preq->preq_src_seqno == rtback->rt_dst_seqno)&&(preq->preq_airtime > rtback->rt_airtime))
	  )
	{
		Packet::free(p);
		return;
	}
	/*
	 * 802.11s 11A.8.5.3.2 Effect of receipt:
	 */
	rtnext = rtable.rtable_lookup(ch->prev_hop_);
	if(rtnext == NULL)
		rtnext = rtable.rtable_add(ch->prev_hop_);
	if(preq->preq_flags & PREQ_F_PROACTIVE)
	{
		/*
		 * received a PREQ from root MP.
		 * 802.11s says nothing about 2 or more roots, so we'll simply
		 * drop any PREQs from other roots.
		 * Otherwise, a routing loop is possible.:(
		 * In other words, we choose the first root MP we've heard
		 * about. Note, that choosing the closest root MP is not a good
		 * idea, because loops will be still possible (especially in
		 * large topologies with 3 or more roots).
		 * TODO: implement other solutions and let the user choose them
		 * from tcl script
		 */
		HDPRINTF("Received proactive PREQ; current mode is %s, preq_src = %d, root_node = %d\n",
				(mode == PROACTIVE)?"PROACTIVE":"REACTIVE",
				preq->preq_src,
				root_node);
		if((mode == PROACTIVE) && (preq->preq_src != root_node))
		{
			Packet::free(p);
			return;
		}
		mode = PROACTIVE;
		root_node = preq->preq_src;
		HDPRINTF("Proactive mode started!, metric = %2.4f, %d",
				preq->preq_airtime,
				preq->preq_hop_count);

		// 1. update route to root
		rt_update(
				rtback,
				preq->preq_src_seqno,
				preq->preq_id,
				preq->preq_hop_count,
				preq->preq_airtime,
				preq->preq_lifetime,
				ch->prev_hop_,
				MAC_BROADCAST,
				IS_TO_ROOT);
		// 1a. Update reactive route to my neighbor
		rt_update(
				rtnext,
				preq->preq_src_seqno,
				preq->preq_id,
				1,
				(CURRENT_TIME-preq->preq_timestamp)*AIR_MULTIPLIER+AIR_OVERHEAD,
				preq->preq_lifetime,
				ch->prev_hop_,
				MAC_BROADCAST,
				NOT_TO_ROOT);
		// 2. send PREP back to root
		HDPRINTF("Proactive PREP initiate!, metric = %2.4f, %d",
				preq->preq_airtime,
				preq->preq_hop_count);

		//TODO: Check the bit if we are allowed to send data or
		//PREP and DATA (proactive PREP bit)
		Packet *buf_pkt;
		while((buf_pkt = rqueue.deque()))
			forward(rtback, buf_pkt);
#ifdef MYTRACE
		printf("%d: %d->%d\n", preq->preq_id, index_, ch->prev_hop_);
#endif
		seqno ++;
		if(seqno < preq->preq_src_seqno)
			seqno = preq->preq_src_seqno;

		sendPREP(
			preq->preq_src,
			preq->preq_src_seqno,
			index_,
			seqno,
			0,
			0,
			preq->preq_lifetime);
		// 3. forward PREQ further
		preq->preq_hop_count	++;
		ch->addr_type()		= NS_AF_INET;
		ch->ptype()		= PT_HWMP;
		ch->direction()		= hdr_cmn::DOWN;
		ch->prev_hop_		= index_;
		ch->num_forwards()	++;
#ifdef MYTRACE
		printf("PREQ:index:%d\n", index_);
#endif
		Scheduler::instance().schedule(target_, p, 0.0);
		return;
	}
	rtforw = rtable.rtable_lookup(preq->preq_dst[0]);
	if(rtforw != NULL)
	{
		if(rtforw->rt_lifetime < CURRENT_TIME)
			rtforw->rt_flags = RTF_OLD;
	}
	else
	{
		rtforw = rtable.rtable_add(preq->preq_dst[0]);
		rtforw->rt_prev_hop = ch->prev_hop_;
		rtforw->rt_flags = RTF_NOPREP;
	}
	rt_update(
			rtback,
			preq->preq_src_seqno,
			preq->preq_id,
			preq->preq_hop_count,
			preq->preq_airtime,
			preq->preq_lifetime,
			ch->prev_hop_,
			rtforw->rt_next_hop, //prev hop. If unknown - broadcast
			NOT_TO_ROOT);
	rt_update(
			rtnext,
			preq->preq_src_seqno,
			preq->preq_id,
			1,
			(CURRENT_TIME-preq->preq_timestamp)*AIR_MULTIPLIER + AIR_OVERHEAD,
			preq->preq_lifetime,
			ch->prev_hop_,
			MAC_BROADCAST,
			NOT_TO_ROOT);
	Packet *buf_pkt;
	while((buf_pkt = rqueue.deque(rtback->rt_dst)))
		forward(rtback, buf_pkt);
	if(((u_int32_t)(ch->last_hop_)) == index_) {
		HDPRINTF("Delivered. PREP, metric = %2.4f, %d",
				preq->preq_airtime,
				preq->preq_hop_count);
		seqno ++;
		if(seqno < preq->preq_src_seqno)
			seqno = preq->preq_src_seqno;

		sendPREP(
			preq->preq_src,
			preq->preq_src_seqno+1,
			index_,
			seqno,
			0,
			0,
			preq->preq_lifetime);
		Packet::free(p);
		return;
	}
	if((rtforw->rt_flags == RTF_ACTUAL) && !(preq->preq_dst_flags[0] & PREQ_DO)) {
		HDPRINTF("Delivered. Intermediate PREP to %d about %d, metric = %2.4f, %d",
			preq->preq_src,
			preq->preq_dst[0],
			preq->preq_airtime,
			preq->preq_hop_count);
		seqno ++;
		if(seqno < preq->preq_src_seqno)
			seqno = preq->preq_src_seqno;
		sendPREP(
				preq->preq_src,
				preq->preq_src_seqno,
				preq->preq_dst[0],
				rtforw->rt_dst_seqno,
				rtforw->rt_airtime,
				rtforw->rt_hop_count,
				preq->preq_lifetime);
		if(!(preq->preq_dst_flags[0] & PREQ_RF)){
			Packet::free(p);
			return;
		}
	}


	/*
	 * Case D:
	 */
	if(rtforw->rt_flags == RTF_ACTUAL)
	{
		//set DO = 1; Case D2 only
		preq->preq_dst_flags[0]	|= PREQ_DO;
		preq->preq_dst_flags[0]	&= ~PREQ_RF; // opinion by Hippo && Glavbot
	}
	/*
	 * Case D2,D1:
	 */
	preq->preq_timestamp	= CURRENT_TIME;
	ch->prev_hop_		= index_;
	ch->next_hop_		= MAC_BROADCAST;
	ch->iface()		= -2; //any
	ch->error()		= 0;
	ch->addr_type()		= NS_AF_INET;
	ch->ptype()		= PT_HWMP;
	ch->direction()		= hdr_cmn::DOWN;
	ch->num_forwards()	++;

	HDPRINTF("forward it, metric = %2.4f, %d",
			preq->preq_airtime,
			preq->preq_hop_count);
#ifdef MYTRACE
	printf("PREQ:index:%d\n", index_);
#endif
	if(rtable.rtable_preq_limit < CURRENT_TIME)
		rtable.rtable_preq_limit = CURRENT_TIME;
	else
		preq->preq_timestamp -= rtable.rtable_preq_limit - CURRENT_TIME;
	Scheduler::instance().schedule(target_, p, rtable.rtable_preq_limit - CURRENT_TIME);
	rtable.rtable_preq_limit += PREQ_DISTANCE;
	return;
}


void
HWMP::rt_update(
		hwmp_rt_entry	*rt,
		u_int32_t	dstseqno,
		u_int32_t	preqid,
		u_int16_t	hopcount,
		double		metric,
		double		lifetime,
		u_int32_t	nexthop,
		u_int32_t	prevhop,
		u_int8_t	istoroot
	       )
{
	// reset flags:
	assert(rt);
	rt->rt_flags	= RTF_ACTUAL;
	rt->rt_retry	= 0;
	// Update routing info:
	rt->rt_dst_seqno	= dstseqno;
	rt->rt_dst_preqID	= preqid;
	rt->rt_hop_count	= hopcount;
	rt->rt_airtime		= metric;
	rt->rt_lifetime		= lifetime;
	rt->rt_next_hop		= nexthop;
	rt->rt_prev_hop		= prevhop;
	rt->rt_to_root		= istoroot;
}

void
HWMP::sendPREP(
		u_int32_t	dst,
		u_int32_t	dst_seqno,
		u_int32_t	src,
		u_int32_t	src_seqno,
		double		metric,
		u_int8_t	hops,
		double		lifetime)

{
	Packet			*p		= Packet::alloc();
	struct hdr_hwmp_prep	*prep		= HDR_HWMP_PREP(p);
	struct hdr_hwmp		*hh		= HDR_HWMP(p);
	struct hdr_cmn		*ch		= HDR_CMN(p);
	struct hdr_ip		*ip		= HDR_IP(p);
	hwmp_rt_entry		*rt;
	rt = rtable.rtable_lookup(dst);
	if(rt == NULL)
	{
		HDPRINTF("ERROR: entry must exist!");
		Packet::free(p);
		return;
	}
	prep_id ++;

	/*
	 * 802.11s 11A.8.6.2  Conditions for generating and sending a PREP
	 * Case A  Original transmission
	 */

	// 1. Filling common header:
	ch->size()		= prep->size();
	ch->iface()		= -2;
	ch->error()		= 0;
	ch->addr_type()		= NS_AF_INET;
	ip->sport()		= RT_PORT;
	ip->dport()		= RT_PORT;
	ch->prev_hop_		= index_;
	ch->next_hop_		= rt->rt_next_hop;
	ch->last_hop_		= dst;
	ch->ptype()		= PT_HWMP;
	ch->direction()		= hdr_cmn::DOWN;
	ch->num_forwards()	= 0;
	prep->prep_timestamp	= CURRENT_TIME;

	// 2. Filling PREP header:
	hh->hwmp_type		= HWMPTYPE_PREP;
	prep->prep_flags	= 0;
	prep->prep_hop_count	= hops;
	hh->hwmp_ttl		= MAX_RT_TTL;
	prep->prep_dst		= dst;
	prep->prep_dst_seqno	= dst_seqno;
	prep->prep_id		= prep_id;
	prep->prep_src		= src;
	prep->prep_src_seqno	= src_seqno;
	prep->prep_lifetime	= lifetime;
	prep->prep_airtime	= metric;

	HDPRINTF("to %d, next hop is %d",
			prep->prep_dst,
			ch->next_hop_);
	Scheduler::instance().schedule(target_, p, 0.0);

	/*
	 * 802.11s 11A.8.6.2  Conditions for generating and sending a PREP
	 * Case B  -- look in recvPREP()
	 */
}

void
HWMP::recvPREP (Packet *p)
{
	struct hdr_hwmp_prep	*prep		= HDR_HWMP_PREP(p);
	struct hdr_cmn		*ch		= HDR_CMN(p);
	hwmp_rt_entry		*rtback, *rtforw, *rtnext;
	prep->prep_hop_count ++;
	prep->prep_airtime	+= (CURRENT_TIME - prep->prep_timestamp) * AIR_MULTIPLIER
		+ AIR_OVERHEAD;
	/*
	 * 802.11s 11A.8.6.3.1 Acceptance criteria
	 */
	rtback = rtable.rtable_lookup(prep->prep_src);
	if(rtback == NULL)
		rtback = rtable.rtable_add(prep->prep_src);
	//drop old PREPs and PREPs with bad airtime metric
	if((prep->prep_src_seqno < rtback->rt_dst_seqno)
			|| ((prep->prep_src_seqno == rtback->rt_dst_seqno)&&(prep->prep_airtime > rtback->rt_airtime))
	  )
	{
		HDPRINTF("Received PREP to %d. Bad seqno or metric",
				prep->prep_dst);
		Packet::free(p);
		return;
	}
	HDPRINTF("Received PREP to %d, metric = %2.4f, %d",
			prep->prep_dst,
			prep->prep_airtime,
			prep->prep_hop_count);
	rtforw = rtable.rtable_lookup(prep->prep_dst);
	if((rtforw == NULL) && (prep->prep_dst != index_))
	{
		HDPRINTF("Error! Here must be an entry!");
		Packet::free(p);
		return;
	}
	rtnext = rtable.rtable_lookup(ch->prev_hop_);
	if(rtnext == NULL)
		rtnext = rtable.rtable_add(ch->prev_hop_);
	if(prep->prep_dst == index_)
	{
		rtback = rtable.rtable_lookup(prep->prep_src);
		// Actually, only if we are in proactive mode we shall not have a routing entry
		if(mode == PROACTIVE)
			rtback = rtable.rtable_add(prep->prep_src);
		rt_update(
				rtback,
				prep->prep_src_seqno,
				prep->prep_id,
				prep->prep_hop_count,
				prep->prep_airtime,
				prep->prep_lifetime,
				ch->prev_hop_,
				index_,
				(mode == REACTIVE)? IS_TO_ROOT:NOT_TO_ROOT);
		rt_update(
				rtnext,
				prep->prep_src_seqno,
				prep->prep_id,
				1,
				(CURRENT_TIME - prep->prep_timestamp) * AIR_MULTIPLIER + AIR_OVERHEAD,
				prep->prep_lifetime,
				ch->prev_hop_,
				index_,
				NOT_TO_ROOT);

		HDPRINTF("PREP Delivered! metric = %2.4f, %d",
				prep->prep_airtime,
				prep->prep_hop_count);
		Packet *buf_pkt;
		while((buf_pkt = rqueue.deque(rtback->rt_dst)))
			forward(rtback, buf_pkt);
		Packet::free(p);
		return;
	}

	if(rtforw->rt_lifetime < CURRENT_TIME)
	{
		//Drop PREP -- we have no valid information;
		HDPRINTF("Received PREP to %d can not be forwarded!",
				prep->prep_dst);

		sendPREQ(prep->prep_dst);
		rqueue.enque(p);
		return;
	}
	// First of all update path to sestination
	rtforw->rt_prev_hop = ch->prev_hop_;


	
	/*
	 * 802.11s 11A.8.6.3.1  Effect of receipt:
	 */
	//Update rtback info:
	ch->next_hop_ = rtforw->rt_next_hop;
	rt_update(
			rtback,
			prep->prep_src_seqno,
			prep->prep_id,
			prep->prep_hop_count,
			prep->prep_airtime,
			prep->prep_lifetime,
			ch->prev_hop_,
			ch->next_hop_,
			NOT_TO_ROOT);
	//Update rtnext info:
	rt_update(
			rtnext,
			prep->prep_src_seqno,
			prep->prep_id,
			1,
			(CURRENT_TIME - prep->prep_timestamp) * AIR_MULTIPLIER + AIR_OVERHEAD,
			prep->prep_lifetime,
			ch->prev_hop_,
			MAC_BROADCAST,
			NOT_TO_ROOT);
	Packet *buf_pkt;
	while((buf_pkt = rqueue.deque(rtback->rt_dst)))
		forward(rtback, buf_pkt);

	/*
	 * 802.11s 11A.8.6.2  Conditions for generating and sending a PREP
	 * Case B  PREP propogation
	 */
	prep->prep_timestamp	= CURRENT_TIME;
	ch->prev_hop_		= index_;
	ch->iface()		= -2;
	ch->error()		= 0;
	ch->addr_type()		= NS_AF_INET;
	ch->ptype()		= PT_HWMP;
	ch->num_forwards()	++;
	ch->direction()		= hdr_cmn::DOWN;

	HDPRINTF("Forward PREP to %d, metric %2.4f, %d, next hop %d",
			prep->prep_dst,
			prep->prep_airtime,
			prep->prep_hop_count,
			ch->next_hop_);
	Scheduler::instance().schedule(target_, p, 0.0);
}

int
HWMP::diff_subnet(u_int32_t dst, u_int32_t src) 
{
    char* dstnet = NULL;
    char* srcnet = NULL;
    
    srcnet = Address::instance().get_subnetaddr(src);
	if (srcnet != NULL) {
	    dstnet = Address::instance().get_subnetaddr(dst);
		if (dstnet != NULL) {
			if (strcmp(dstnet, srcnet) != 0) {
			    delete [] srcnet;
				delete [] dstnet;

				return 1;
			}
			delete [] dstnet;
			dstnet = NULL;
		}
		delete [] srcnet;
		srcnet = NULL;
	}
	 
	assert(srcnet == NULL);
	assert(dstnet == NULL);
	return 0;
}

void
ProactivePREQ_timer::handle(Event*) {
	agent->sendPREQ(MAC_BROADCAST);
	Scheduler::instance().schedule(this, &intr, PROACTIVE_TIMESLOT);
}

/*
 * hwmp.h
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
 * $Id: hwmp.h 44 2008-08-04 22:34:52Z hippo $
 */



#include <channel.h>
#include <mac/marshall.h>
#include <address.h>
#include <cmu-trace.h>
#include <priqueue.h>
#include "hwmp_rtable.h"
#include "hwmp_rqueue.h"
#include <classifier/classifier-port.h>

#define HWMP_DEBUG

// HWMP debug printf
#ifdef HWMP_DEBUG
#define	HDPRINTF(...) do {						\
	fprintf(							\
			stderr,						\
			"%s: %s: %4d\tI am %4d, time %2.4f  ",		\
			__FILE__,					\
			__FUNCTION__,					\
			__LINE__,					\
			index_,						\
			CURRENT_TIME);					\
	fprintf(stderr, __VA_ARGS__);					\
	fprintf(stderr, "\n");						\
} while (0)
#else // HWMP_DEBUG
#define HDPRINTF(...)
#endif //HWMP_DEBUG


class HWMP;

class ProactivePREQ_timer : public Handler {
	public:
		ProactivePREQ_timer(HWMP* a) : agent(a) {}
		void	handle(Event*);
	private:
		HWMP	*agent;
		Event	intr;
};

class HWMP : public Agent
{
	friend class hwmp_rt_entry;
	friend class ProactivePREQ_timer;

	public:
		HWMP(u_int32_t macaddr);
		void		recv(Packet* p, Handler *);

	protected:
		u_int32_t	seqno;
		u_int32_t	index_;
		u_int32_t	preq_id;
		u_int32_t	prep_id;
		u_int8_t	mode;
#define REACTIVE		0
#define PROACTIVE		1
		u_int32_t	root_node;
		/*
		 * Timers
		 */
		ProactivePREQ_timer proactivePREQ_timer;

		hwmp_rtable	rtable;
		int		command(int, const char *const *);
		int		initialized() { return 1 && target_; }
		void		recvHWMP(Packet* p);
		void		rt_resolve(Packet *p);
		void		forward(hwmp_rt_entry *rt, Packet *p, double delay);
		void		sendPREQ(u_int32_t dst);
		void		recvPREQ(Packet *p);
		void		sendPREP(
					u_int32_t	dst,
					u_int32_t	dst_seqno,
					u_int32_t	src,
					u_int32_t	src_seqno,
					double		metric,
					u_int8_t	hops,
					double		lifetime);
		void		recvPREP(Packet *p);
		void		rt_update(
					hwmp_rt_entry	*rt,
					u_int32_t	dst_seqno,
					u_int32_t	preqid,
					u_int16_t	hopcount,
					double		metric,
					double		lifetime,
					u_int32_t	nexthop,
					u_int32_t	prevhop,
					u_int8_t	istoroot);
		void		forward(hwmp_rt_entry* rt, Packet* p);

		hwmp_rqueue	rqueue;
		Trace		*logtarget;
		PriQueue	*ifqueue;
		PortClassifier	*dmux_;
		
		int  diff_subnet(u_int32_t dst, u_int32_t src);
		MobileNode* node_;
};

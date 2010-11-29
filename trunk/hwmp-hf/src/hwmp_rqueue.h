/*
 * hwmp_rqueue.h
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
 * $Id: hwmp_rqueue.h 44 2008-08-04 22:34:52Z hippo $
 */



#ifndef __hwmp_rqueue_h__
#define __hwmp_rqueue_h__

#include <agent.h>
#define HWMP_RTQ_MAX_LEN	64 // packets
#define HWMP_RTQ_TIMEOUT	30 // seconds

class hwmp_rqueue : public Connector {
	public:
		hwmp_rqueue();

		void		recv(Packet *, Handler*) { abort(); }
		void		enque(Packet *p);
		inline int	command(int argc, const char * const* argv)
			{ return Connector::command(argc, argv); }

		/*
		 *  Returns a packet from the head of the queue.
		 */
		Packet*		deque(void);

		/*
		 * Returns a packet for destination "dst".
		 */
		Packet*		deque(u_int32_t dst);

		/*
		 * Finds whether a packet with destination "dst" exists in the queue
		 */
		bool		find(u_int32_t dst);


	private:
		Packet*		remove_head();
		void		purge(void);
		void		findPacketWithDst(u_int32_t dst, Packet*& p, Packet*& prev);
		bool		findAgedPacket(Packet*& p, Packet*& prev);
		void		verifyQueue(void);

		Packet*		head_;
		Packet*		tail_;

		int		len_;

		int		limit_;
		double		timeout_;
};

#endif /* __hwmp_rqueue_h__ */

/*
 * hwmp_rqueue.cc
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
 * $Id: hwmp_rqueue.cc 44 2008-08-04 22:34:52Z hippo $
 */



#include <assert.h>

#include <cmu-trace.h>
#include "hwmp_rqueue.h"
#include <mac/mac.h>


#define CURRENT_TIME	Scheduler::instance().clock()
//#define QDEBUG

/*
   Packet Queue used by HWMP.
 */

hwmp_rqueue::hwmp_rqueue() {
	head_		= tail_ = 0;
	len_		= 0;
	limit_		= HWMP_RTQ_MAX_LEN;
	timeout_	= HWMP_RTQ_TIMEOUT;
}

void
hwmp_rqueue::enque(Packet *p) {
	struct hdr_cmn *ch = HDR_CMN(p);

	/*
	 * Purge any packets that have timed out.
	 */
	purge();

	p->next_	= 0;
	ch->ts_		= CURRENT_TIME + timeout_;

	if (len_ == limit_) {
		Packet *p0 = remove_head();	// decrements len_

		assert(p0);
		if(HDR_CMN(p0)->ts_ > CURRENT_TIME) {
			drop(p0, DROP_RTR_QFULL);
		}
		else {
			drop(p0, DROP_RTR_QTIMEOUT);
		}
	}

	if(head_ == 0) {
		head_ = tail_ = p;
	}
	else {
		tail_->next_ = p;
		tail_ = p;
	}
	len_++;
#ifdef QDEBUG
	verifyQueue();
#endif // QDEBUG
}


Packet*
hwmp_rqueue::deque() {
	Packet *p;

	/*
	 * Purge any packets that have timed out.
	 */
	purge();

	p = remove_head();
#ifdef QDEBUG
	verifyQueue();
#endif // QDEBUG
	return p;
}


Packet*
hwmp_rqueue::deque(u_int32_t dst) {
	Packet *p, *prev;

	/*
	 * Purge any packets that have timed out.
	 */
	purge();

	findPacketWithDst(dst, p, prev);
	assert((p == 0) || ((p == head_) && (prev == 0)) || (prev->next_ == p));

	if(p == 0) return NULL;

	if (p == head_) {
		p = remove_head();
	}
	else if (p == tail_) {
		prev->next_	= 0;
		tail_		= prev;
		len_--;
	}
	else {
		prev->next_	= p->next_;
		len_--;
	}

#ifdef QDEBUG
	verifyQueue();
#endif // QDEBUG
	return p;
}

bool
hwmp_rqueue::find(u_int32_t dst) {
	Packet *p, *prev;

	findPacketWithDst(dst, p, prev);
	if (0 == p)
		return false;
	else
		return true;
}



/*
   Private Routines
 */

Packet*
hwmp_rqueue::remove_head() {
	Packet *p = head_;

	if(head_ == tail_) {
		head_ = tail_ = 0;
	}
	else {
		head_ = head_->next_;
	}

	if(p) len_--;

	return p;
}

void
hwmp_rqueue::findPacketWithDst(u_int32_t dst, Packet*& p, Packet*& prev) {

	p = prev = 0;

	for(p = head_; p; p = p->next_) {
		if(HDR_CMN(p)->last_hop_ == dst) {
			return;
		}
		prev = p;
	}
}


void
hwmp_rqueue::verifyQueue() {
	Packet	*p, *prev	= 0;
	int	cnt		= 0;

	for(p = head_; p; p = p->next_) {
		cnt++;
		prev = p;
	}
	assert(cnt == len_);
	assert(prev == tail_);
}


bool
hwmp_rqueue::findAgedPacket(Packet*& p, Packet*& prev) {
	p = prev = 0;

	for(p = head_; p; p = p->next_) {
		if(HDR_CMN(p)->ts_ < CURRENT_TIME) {
			return true;
		}
		prev = p;
	}
	return false;
}


void
hwmp_rqueue::purge() {
	Packet *p, *prev;

	while ( findAgedPacket(p, prev) ) {
		assert((p == 0) || (p == head_ && prev == 0) || (prev->next_ == p));

		if(p == 0) return;

		if (p == head_) {
			p		= remove_head();
		}
		else if (p == tail_) {
			prev->next_	= 0;
			tail_		= prev;
			len_--;
		}
		else {
			prev->next_	= p->next_;
			len_--;
		}
#ifdef QDEBUG
		verifyQueue();
#endif // QDEBUG

		p = prev = 0;
	}
}


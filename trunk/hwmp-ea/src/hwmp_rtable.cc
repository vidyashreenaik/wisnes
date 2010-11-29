/*
 * hwmp_rtable.cc
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
 * $Id: hwmp_rtable.cc 44 2008-08-04 22:34:52Z hippo $
 */



#include "hwmp_rtable.h"

// rtable_entry routines:
hwmp_rt_entry::hwmp_rt_entry()
{
	rt_airtime	= 0;
	rt_dst		= MAC_BROADCAST;
	rt_retry	= 0;
}

// rtable routines:
hwmp_rt_entry*
hwmp_rtable::rtable_lookup(u_int32_t dst)
{
	hwmp_rt_entry *rt = rthead.lh_first;

	for(; rt; rt = rt->rt_link.le_next)
	{
		if(rt->rt_dst == dst)
			return rt;
	}
	return NULL;
}

hwmp_rt_entry*
hwmp_rtable::rtable_add(u_int32_t adr)
{
	hwmp_rt_entry *rt;
	rt = rtable_lookup(adr);
	if(rt != NULL)
		return rt;
	rt = new hwmp_rt_entry;
	assert(rt);
	rt->rt_dst		= adr;
	rt->rt_dst_seqno	= 0;
	rt->rt_to_root		= NOT_TO_ROOT;
	rt->rt_airtime		= MAX_METRIC;
	rt->rt_flags		= RTF_INIT;
	LIST_INSERT_HEAD(&rthead, rt, rt_link);
	return rt;
}

void
hwmp_rtable::rtable_del(u_int32_t dst)
{
	hwmp_rt_entry *rt = rtable_lookup(dst);

	if(rt != NULL)
	{
		LIST_REMOVE(rt, rt_link);
		delete rt;
	}
}
#if 0
	double
hwmp_rtable::rtable_get_preqdelay(u_int8_t retry)
{
	if(CURRENT_TIME>rtable_preq_limit){
		rtable_preq_limit = CURRENT_TIME+(retry)*PREQ_TIMESLOT;
		return 0.;
	} else
	{
		rtable_preq_limit += (retry)*PREQ_TIMESLOT;
	}
}
#endif


#ifndef __directional_mac_h__
#define __directional_mac_h__

#include <math.h>
#include <stddef.h>
#include <list>
#include <map>

#include "address.h"
#include "ip.h"
#include "marshall.h"
#include "wireless_phy_interface.h"

class EventTrace;

#define GET_ETHER_TYPE(x)		GET2BYTE((x))
#define SET_ETHER_TYPE(x,y)     {u_int16_t t = (y); STORE2BYTE(x,&t);}

/* ======================================================================
 Frame Formats
 ====================================================================== */

#define	MAC_ProtocolVersion	0x00

#define MAC_Type_Management	0x00
#define MAC_Type_Control	0x01
#define MAC_Type_Data		0x02
#define MAC_Type_Reserved	0x03

#define MAC_Subtype_RTS		0x0B
#define MAC_Subtype_CTS		0x0C
#define MAC_Subtype_ACK		0x0D
#define MAC_Subtype_Data	0x00

#define MAC_Subtype_80211_Beacon	0x08
#define MAC_Subtype_AssocReq	0x00
#define MAC_Subtype_AssocRep	0x01
#define MAC_Subtype_Auth	0x0C
#define MAC_Subtype_ProbeReq	0x04
#define MAC_Subtype_ProbeRep	0x05

struct frame_control {
	u_char fc_subtype :4;
	u_char fc_type :2;
	u_char fc_protocol_version :2;

	u_char fc_order :1;
	u_char fc_wep :1;
	u_char fc_more_data :1;
	u_char fc_pwr_mgt :1;
	u_char fc_retry :1;
	u_char fc_more_frag :1;
	u_char fc_from_ds :1;
	u_char fc_to_ds :1;
};

struct rts_frame {
	struct frame_control rf_fc;
	u_int16_t rf_duration;
	u_char rf_ra[ETHER_ADDR_LEN];
	u_char rf_ta[ETHER_ADDR_LEN];
	u_char rf_fcs[ETHER_FCS_LEN];
};

struct cts_frame {
	struct frame_control cf_fc;
	u_int16_t cf_duration;
	u_char cf_ra[ETHER_ADDR_LEN];
	u_char cf_fcs[ETHER_FCS_LEN];
};

struct ack_frame {
	struct frame_control af_fc;
	u_int16_t af_duration;
	u_char af_ra[ETHER_ADDR_LEN];
	u_char af_fcs[ETHER_FCS_LEN];
};

struct beacon_frame {
	struct frame_control bf_fc;
	u_int16_t bf_duration;
	u_char bf_ra[ETHER_ADDR_LEN];
	u_char bf_ta[ETHER_ADDR_LEN];
	u_char bf_3a[ETHER_ADDR_LEN];
	u_int16_t bf_scontrol;
	double bf_timestamp;
	double bf_bcninterval;
	u_int8_t bf_datarates[1];
	u_char bf_fcs[ETHER_FCS_LEN];
};

struct assocreq_frame {
	struct frame_control acrqf_fc;
	u_int16_t acrqf_duration;
	u_char acrqf_ra[ETHER_ADDR_LEN];
	u_char acrqf_ta[ETHER_ADDR_LEN];
	u_char acrqf_3a[ETHER_ADDR_LEN];
	u_int16_t acrqf_scontrol;
	u_char acrqf_fcs[ETHER_FCS_LEN];
};

struct assocrep_frame {
	struct frame_control acrpf_fc;
	u_int16_t acrpf_duration;
	u_char acrpf_ra[ETHER_ADDR_LEN];
	u_char acrpf_ta[ETHER_ADDR_LEN];
	u_char acrpf_3a[ETHER_ADDR_LEN];
	u_int16_t acrpf_scontrol;
	u_int16_t acrpf_statuscode;
	u_char acrpf_fcs[ETHER_FCS_LEN];
};

struct auth_frame {
	struct frame_control authf_fc;
	u_int16_t authf_duration;
	u_char authf_ra[ETHER_ADDR_LEN];
	u_char authf_ta[ETHER_ADDR_LEN];
	u_char authf_3a[ETHER_ADDR_LEN];
	u_int16_t authf_scontrol;
	u_int16_t authf_algono;
	u_int16_t authf_seqno;
	u_int16_t authf_statuscode;
	u_char authf_fcs[ETHER_FCS_LEN];
};

struct probereq_frame {
	struct frame_control prrqf_fc;
	u_int16_t prrqf_duration;
	u_char prrqf_ra[ETHER_ADDR_LEN];
	u_char prrqf_ta[ETHER_ADDR_LEN];
	u_char prrqf_3a[ETHER_ADDR_LEN];
	u_int16_t prrqf_scontrol;
	u_char prrqf_fcs[ETHER_FCS_LEN];
};

struct proberep_frame {
	struct frame_control prrpf_fc;
	u_int16_t prrpf_duration;
	u_char prrpf_ra[ETHER_ADDR_LEN];
	u_char prrpf_ta[ETHER_ADDR_LEN];
	u_char prrpf_3a[ETHER_ADDR_LEN];
	u_int16_t prrpf_scontrol;
	double prrpf_timestamp;
	double prrpf_bcninterval;
	u_int8_t prrpf_datarates[1];
	u_char prrpf_fcs[ETHER_FCS_LEN];
};

// This header does not have its header access function because it shares
// the same header space with hdr_mac.
struct hdr_mac802_11 {
	struct frame_control dh_fc;
	u_int16_t dh_duration;
	u_char dh_ra[ETHER_ADDR_LEN];
	u_char dh_ta[ETHER_ADDR_LEN];
	u_char dh_3a[ETHER_ADDR_LEN];
	u_char dh_4a[ETHER_ADDR_LEN];
	u_int16_t dh_scontrol;
	u_char dh_body[1]; // size of 1 for ANSI compatibility
};

struct client_table {
	int client_id;
	int auth_status;
	int assoc_status;
};

struct ap_table {
	int ap_id;
	double ap_power;
};

struct priority_queue {
	int frame_priority;
	struct priority_queue *next;
};

/* ======================================================================
 Timers
 ====================================================================== */
class Mac802_11DVCS;

class MacTimer: public Handler {
public:
	MacTimer(Mac802_11DVCS* m) :
		mac(m) {
		busy_ = paused_ = 0;
		stime = rtime = 0.0;
	}

	virtual void handle(Event *e) = 0;

	virtual void start(double time);
	virtual void stop(void);
	virtual void pause(void) {
		assert(0);
	}
	virtual void resume(void) {
		assert(0);
	}

	inline int busy(void) {
		return busy_;
	}
	inline int paused(void) {
		return paused_;
	}
	inline double expire(void) {
		return ((stime + rtime) - Scheduler::instance().clock());
	}

protected:
	Mac802_11DVCS *mac;
	int busy_;
	int paused_;
	Event intr;
	double stime; // start time
	double rtime; // remaining time
};

class BackoffTimer: public MacTimer {
public:
	BackoffTimer(Mac802_11DVCS *m) :
		MacTimer(m), difs_wait(0.0) {
	}

	void start(int cw, int idle, double difs = 0.0);
	void handle(Event *e);
	void pause(void);
	void resume(double difs);
private:
	double difs_wait;
};

class DeferTimer: public MacTimer {
public:
	DeferTimer(Mac802_11DVCS *m) :
		MacTimer(m) {
	}

	void start(double);
	void handle(Event *e);
};

class BeaconTimer: public MacTimer {
public:
	BeaconTimer(Mac802_11DVCS *m) :
		MacTimer(m) {
	}

	void start(double);
	void handle(Event *e);
};

class ProbeTimer: public MacTimer {
public:
	ProbeTimer(Mac802_11DVCS *m) :
		MacTimer(m) {
	}

	void start(double);
	void handle(Event *e);
};

class IFTimer: public MacTimer {
public:
	IFTimer(Mac802_11DVCS *m) :
		MacTimer(m) {
	}

	void handle(Event *e);
};

class NavTimer: public MacTimer {
public:
	NavTimer(Mac802_11DVCS *m) :
		MacTimer(m) {
	}

	void handle(Event *e);
};

class RxTimer: public MacTimer {
public:
	RxTimer(Mac802_11DVCS *m) :
		MacTimer(m) {
	}

	void handle(Event *e);
};

class TxTimer: public MacTimer {
public:
	TxTimer(Mac802_11DVCS *m) :
		MacTimer(m) {
	}

	void handle(Event *e);
};

class DirectionalNav;

class DirectionalNavTimer: public Handler {
public:
	DirectionalNavTimer(DirectionalNav* directionalNav) :
		directionalNav_(directionalNav) {
		busy_ = 0;
		stime = rtime = 0.0;
	}

	virtual void handle(Event *e);

	virtual void start(double time);
	virtual void stop(void);

	inline int busy(void) {
		return busy_;
	}

	inline double expire(void) {
		return ((stime + rtime) - Scheduler::instance().clock());
	}

protected:
	DirectionalNav *directionalNav_;
	int busy_;
	Event intr;
	double stime; // start time
	double rtime; // remaining time
};
/* ======================================================================
 Definitions
 ====================================================================== */

/* Must account for propagation delays added by the channel model when
 * calculating tx timeouts (as set in tcl/lan/ns-mac.tcl).
 *   -- Gavin Holland, March 2002
 */
#define DSSS_MaxPropagationDelay        0.000002        // 2us   XXXX
class PHY_MIB {
public:
	PHY_MIB(Mac802_11DVCS *parent);

	inline u_int32_t getCWMin() {
		return (CWMin);
	}
	inline u_int32_t getCWMax() {
		return (CWMax);
	}
	inline double getSlotTime() {
		return (SlotTime);
	}
	inline double getBeaconInterval() {
		return (BeaconInterval);
	}
	inline double getSIFS() {
		return (SIFSTime);
	}
	inline double getPIFS() {
		return (SIFSTime + SlotTime);
	}
	inline double getDIFS() {
		return (SIFSTime + 2 * SlotTime);
	}
	inline double getEIFS() {
		// see (802.11-1999, 9.2.10)
		return (SIFSTime + getDIFS() + (8 * getACKlen()) / PLCPDataRate);
	}
	inline u_int32_t getPreambleLength() {
		return (PreambleLength);
	}
	inline double getPLCPDataRate() {
		return (PLCPDataRate);
	}

	inline u_int32_t getPLCPhdrLen() {
		return ((PreambleLength + PLCPHeaderLength) >> 3);
	}

	inline u_int32_t getHdrLen11() {
		return (getPLCPhdrLen() + offsetof(struct hdr_mac802_11, dh_body[0])
				+ ETHER_FCS_LEN);
	}

	inline u_int32_t getRTSlen() {
		return (getPLCPhdrLen() + sizeof(struct rts_frame));
	}

	inline u_int32_t getCTSlen() {
		return (getPLCPhdrLen() + sizeof(struct cts_frame));
	}

	inline u_int32_t getACKlen() {
		return (getPLCPhdrLen() + sizeof(struct ack_frame));
	}
	inline u_int32_t getBEACONlen() {
		return (getPLCPhdrLen() + sizeof(struct beacon_frame));
	}
	inline u_int32_t getASSOCREQlen() {
		return (getPLCPhdrLen() + sizeof(struct assocreq_frame));
	}
	inline u_int32_t getASSOCREPlen() {
		return (getPLCPhdrLen() + sizeof(struct assocrep_frame));
	}
	inline u_int32_t getAUTHENTICATElen() {
		return (getPLCPhdrLen() + sizeof(struct auth_frame));
	}
	inline u_int32_t getPROBEREQlen() {
		return (getPLCPhdrLen() + sizeof(struct probereq_frame));
	}
	inline u_int32_t getPROBEREPlen() {
		return (getPLCPhdrLen() + sizeof(struct proberep_frame));
	}
private:

	u_int32_t CWMin;
	u_int32_t CWMax;
	double SlotTime;
	double SIFSTime;
	double BeaconInterval;
	u_int32_t PreambleLength;
	u_int32_t PLCPHeaderLength;
	double PLCPDataRate;
};

/*
 * IEEE 802.11 Spec, section 11.4.4.2
 *      - default values for the MAC Attributes
 */
#define MAC_FragmentationThreshold	2346		// bytes
#define MAC_MaxTransmitMSDULifetime	512		// time units
#define MAC_MaxReceiveLifetime		512		// time units
class MAC_MIB {
public:

	MAC_MIB(Mac802_11DVCS *parent);

private:
	u_int32_t RTSThreshold;
	u_int32_t ShortRetryLimit;
	u_int32_t LongRetryLimit;
	u_int32_t ScanType;
	double ProbeDelay;
	double MaxChannelTime;
	double MinChannelTime;
	double ChannelTime;

public:
	u_int32_t FailedCount;
	u_int32_t RTSFailureCount;
	u_int32_t ACKFailureCount;
public:
	inline u_int32_t getRTSThreshold() {
		return (RTSThreshold);
	}
	inline u_int32_t getShortRetryLimit() {
		return (ShortRetryLimit);
	}
	inline u_int32_t getLongRetryLimit() {
		return (LongRetryLimit);
	}
	inline u_int32_t getScanType() {
		return (ScanType);
	}
	inline double getProbeDelay() {
		return (ProbeDelay);
	}
	inline double getMaxChannelTime() {
		return (MaxChannelTime);
	}
	inline double getMinChannelTime() {
		return (MinChannelTime);
	}
	inline double getChannelTime() {
		return (ChannelTime);
	}
};

/* ======================================================================
 The following destination class is used for duplicate detection.
 ====================================================================== */
class Host {
public:
	LIST_ENTRY(Host) link;
	u_int32_t index;
	u_int32_t seqno;
};

/* ======================================================================
 DVCS
 ====================================================================== */
class DirectionalNav {
	friend class DirectionalNavTimer;
public:
	DirectionalNav(double width, double direction, double expirationTime);
	bool isCovering(double direction) const;
	inline double getWidth() const {
		return width_;
	}
	inline double getDirection() const {
		return direction_;
	}
	inline u_int16_t getExpirationTime() const {
		return expirationTime_;
	}
	inline void setExpirationTime(u_int16_t us) {
		double now = Scheduler::instance().clock();
		double t = us * 1e-6;
		if ((now + t) > expirationTime_) {
			expirationTime_ = now + t;
			if (navTimer_.busy())
				navTimer_.stop();
			navTimer_.start(t);
		}
	}
protected:
	void timerHandler();
private:
	DirectionalNavTimer navTimer_;
	double width_;
	double direction_;
	u_int16_t expirationTime_;
};

/* ======================================================================
 The actual 802.11 MAC class.
 ====================================================================== */
class Mac802_11DVCS: public Mac {
	friend class DeferTimer;

	friend class BeaconTimer;
	friend class ProbeTimer;
	friend class BackoffTimer;
	friend class IFTimer;
	friend class NavTimer;
	friend class RxTimer;
	friend class TxTimer;
public:
	Mac802_11DVCS();
	void recv(Packet *p, Handler *h);
	inline int hdr_dst(char* hdr, int dst = -2);
	inline int hdr_src(char* hdr, int src = -2);
	inline int hdr_type(char* hdr, u_int16_t type = 0);

	inline int bss_id() {
		return bss_id_;
	}

	// Added by Sushmita to support event tracing
	void trace_event(char *, Packet *);
	EventTrace *et_;

protected:
	void backoffHandler(void);
	void deferHandler(void);
	void BeaconHandler(void);
	void ProbeHandler(void);
	void navHandler(void);
	void recvHandler(void);
	void sendHandler(void);
	void txHandler(void);

private:
	void update_client_table(int num, int auth_status, int assoc_status);
	int find_client(int num);
	void update_ap_table(int num, double power);
	int strongest_ap();
	int find_ap(int num, double power);
	void deletelist();
	void passive_scan();
	void active_scan();
	void checkAssocAuthStatus();
	int command(int argc, const char* const * argv);

	void add_priority_queue(int num);
	void push_priority(int num);
	void delete_lastnode();
	void shift_priority_queue();

	/* In support of bug fix described at
	 * http://www.dei.unipd.it/wdyn/?IDsezione=2435
	 */
	int bugFix_timer_;
	int infra_mode_;
	double BeaconTxtime_;
	int associated;
	int authenticated;
	int handoff;
	double Pr;
	int ap_temp;
	int ap_addr;
	int tx_mgmt_;
	int associating_node_;
	int authenticating_node_;
	int ScanType_;
	int OnMinChannelTime;
	int OnMaxChannelTime;
	int Recv_Busy_;
	int probe_delay;
	/*
	 * Called by the timers.
	 */
	void recv_timer(void);
	void send_timer(void);
	int check_pktCTRL();
	int check_pktRTS();
	int check_pktTx();
	int check_pktASSOCREQ();
	int check_pktASSOCREP();
	int check_pktBEACON();
	int check_pktAUTHENTICATE();
	int check_pktPROBEREQ();
	int check_pktPROBEREP();

	/*
	 * Packet Transmission Functions.
	 */
	void send(Packet *p, Handler *h);
	void sendRTS(int dst);
	void sendCTS(int dst, double duration);
	void sendACK(int dst);
	void sendDATA(Packet *p);
	void sendBEACON(int src);
	void sendASSOCREQ(int dst);
	void sendASSOCREP(int dst);
	void sendPROBEREQ(int dst);
	void sendPROBEREP(int dst);
	void sendAUTHENTICATE(int dst);
	void RetransmitRTS();
	void RetransmitDATA();
	void RetransmitASSOCREP();
	void RetransmitPROBEREP();
	void RetransmitAUTHENTICATE();

	/*
	 * Packet Reception Functions.
	 */
	void recvRTS(Packet *p);
	void recvCTS(Packet *p);
	void recvACK(Packet *p);
	void recvDATA(Packet *p);
	void recvBEACON(Packet *p);
	void recvASSOCREQ(Packet *p);
	void recvASSOCREP(Packet *p);
	void recvPROBEREQ(Packet *p);
	void recvPROBEREP(Packet *p);
	void recvAUTHENTICATE(Packet *p);

	void capture(Packet *p);
	void collision(Packet *p);
	void discard(Packet *p, const char* why);
	void rx_resume(void);
	void tx_resume(void);

	inline int is_idle(void);

	/*
	 * Debugging Functions.
	 */
	void trace_pkt(Packet *p);
	void dump(char* fname);

	inline int initialized() {
		return (cache_ && logtarget_ && Mac::initialized());
	}

	inline void mac_log(Packet *p) {
		logtarget_->recv(p, (Handler*) 0);
	}

	double txtime(Packet *p);
	double txtime(double psz, double drt);
	double txtime(int bytes) { /* clobber inherited txtime() */
		abort();
		return 0;
	}

	inline void transmit(Packet *p, double timeout);
	inline void checkBackoffTimer(void);
	inline void postBackoff(int pri);
	inline void setRxState(MacState newState);
	inline void setTxState(MacState newState);

	inline void inc_cw() {
		cw_ = (cw_ << 1) + 1;
		if (cw_ > phymib_.getCWMax())
			cw_ = phymib_.getCWMax();
	}
	inline void rst_cw() {
		cw_ = phymib_.getCWMin();
	}

	inline double sec(double t) {
		return (t *= 1.0e-6);
	}
	inline u_int16_t usec(double t) {
		u_int16_t us = (u_int16_t) floor((t *= 1e6) + 0.5);
		return us;
	}
	inline void set_nav(u_int16_t us) {
		double now = Scheduler::instance().clock();
		double t = us * 1e-6;
		if ((now + t) > nav_) {
			nav_ = now + t;
			if (mhNav_.busy())
				mhNav_.stop();
			mhNav_.start(t);
		}
	}

	inline int isIdle(double direction) {
		if (rx_state_ != MAC_IDLE)
			return 0;
		if (tx_state_ != MAC_IDLE)
			return 0;

		map<double, DirectionalNav *>::iterator iter = directionalNavs_.begin();
		while (iter != directionalNavs_.end()) {
			DirectionalNav* directionalNav = (*iter).second;
			// If any of the directional NAV is busy
			// and it covers the given direction
			if (directionalNav->isCovering(direction)) {
				if (directionalNav->getExpirationTime()
						> Scheduler::instance().clock()) {

					return 0; // return false
				}
			}
			++iter;
		}

		return 1;
	}

	inline void setDirectionalNav(double direction, u_int16_t us) {
		map<double, DirectionalNav *>::iterator iter = directionalNavs_.find(
				direction);
		if (iter == directionalNavs_.end()) {
			DirectionalNav* directionalNav = new DirectionalNav(60, direction,
					us);
			directionalNavs_[direction] = directionalNav;
		} else {
			(*iter).second->setExpirationTime(us);
		}
	}

protected:
	PHY_MIB phymib_;
	MAC_MIB macmib_;

	/* the macaddr of my AP in BSS mode; for IBSS mode
	 * this is set to a reserved value IBSS_ID - the
	 * MAC_BROADCAST reserved value can be used for this
	 * purpose
	 */
	int bss_id_;
	enum {
		IBSS_ID = MAC_BROADCAST
	};
	enum {
		PASSIVE = 0, ACTIVE = 1
	};

private:
	double basicRate_;
	double dataRate_;
	struct client_table *client_list;
	struct ap_table *ap_list;
	struct priority_queue *queue_head;

	/*
	 * Mac Timers
	 */
	IFTimer mhIF_; // interface timer
	NavTimer mhNav_; // NAV timer
	RxTimer mhRecv_; // incoming packets
	TxTimer mhSend_; // outgoing packets

	DeferTimer mhDefer_; // defer timer
	BackoffTimer mhBackoff_; // back-off timer
	BeaconTimer mhBeacon_; // Beacon Timer
	ProbeTimer mhProbe_; //Probe timer,

	/* ============================================================
	 Internal MAC State
	 ============================================================ */
	double nav_; // Network Allocation Vector

	MacState rx_state_; // incoming state (MAC_RECV or MAC_IDLE)
	MacState tx_state_; // outgoing state
	int tx_active_; // transmitter is ACTIVE

	Packet *eotPacket_; // copy for EOT call-back

	Packet *pktRTS_; // outgoing RTS packet
	Packet *pktCTRL_; // outgoing non-RTS packet
	Packet *pktBEACON_; //outgoing Beacon Packet
	Packet *pktASSOCREQ_; //Association request
	Packet *pktASSOCREP_; // Association response
	Packet *pktAUTHENTICATE_; //Authentication
	Packet *pktPROBEREQ_; //Probe Request
	Packet *pktPROBEREP_; //Probe Response

	u_int32_t cw_; // Contention Window
	u_int32_t ssrc_; // STA Short Retry Count
	u_int32_t slrc_; // STA Long Retry Count

	int min_frame_len_;

	NsObject* logtarget_;
	NsObject* EOTtarget_; // given a copy of packet at TX end


	/* ============================================================
	 Duplicate Detection state
	 ============================================================ */
	u_int16_t sta_seqno_; // next seqno that I'll use
	int cache_node_count_;
	Host *cache_;

	std::list<struct client_table> client_list1;
	std::list<struct ap_table> ap_list1;

	std::list<DirectionalNavTimer *> navTimers_;
	std::map<double, DirectionalNav *> directionalNavs_;
};

#endif /* __directional_mac_h__ */

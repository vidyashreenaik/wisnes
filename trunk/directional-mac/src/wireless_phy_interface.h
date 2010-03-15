#ifndef __wireless_phy_interface_h__
#define __wireless_phy_interface_h__

#include "propagation.h"
#include "modulation.h"
#include "omni-antenna.h"
#include "phy.h"
#include "mobilenode.h"
#include "timer-handler.h"

class Phy;
class WirelessPhyInterface;
class Propagation;

class WirelessPhyInterfaceSleepTimer: public TimerHandler {
public:
	WirelessPhyInterfaceSleepTimer(WirelessPhyInterface *a) :
		TimerHandler() {
		a_ = a;
	}
protected:
	virtual void expire(Event *e);
	WirelessPhyInterface *a_;
};

class WirelessPhyInterface: public Phy {
	friend class WirelessPhyInterfaceSleepTimer;

public:
	WirelessPhyInterface();

	void sendDown(Packet *p);
	int sendUp(Packet *p);

	inline double getL() const {
		return L_;
	}

	inline double getLambda() const {
		return lambda_;
	}

	inline double getPtConsume() {
		return Pt_consume_;
	}

	double getDist(double Pr, double Pt, double Gt, double Gr, double hr,
			double ht, double L, double lambda);

	virtual int command(int argc, const char* const * argv);
	virtual void dump(void) const;

	void setInterfaceOn();
	void setInterfaceOff();

	inline Antenna* getAntenna() {
		return ant_;
	}

	inline double getPt() {
		return Pt_;
	}

	inline double getRXThresh() {
		return RXThresh_;
	}

	inline double getCSThresh() {
		return CSThresh_;
	}

	inline double getFreq() {
		return freq_;
	}

	void putInterfaceToSleep();
	void wakeInterfaceUp();

	inline bool isInterfaceOn() {
		return isInterfaceOn_;
	}

	inline bool isSleeping() {
		if (status_ == SLEEP)
			return (1);
		else
			return (0);
	}

	double T_sleep_; // 2.31 change: Time at which sleeping is to be enabled (sec)

protected:
	double Pt_; // transmitted signal power (W)
	double Pt_consume_; // power consumption for transmission (W)
	double Pr_consume_; // power consumption for reception (W)
	double P_idle_; // idle power consumption (W)
	double P_sleep_; // sleep power consumption (W)
	double P_transition_; // power consumed when transmitting from SLEEP mode to IDLE mode and vice versa.
	double T_transition_; // time period to transit from SLEEP mode to IDLE mode and vice versa.
	//

	double last_send_time_; // the last time the node sends something.
	double channel_idle_time_; // channel idle time.
	double update_energy_time_; // the last time we update energy.

	double freq_; // frequency
	double lambda_; // wavelength (m)
	double L_; // system loss factor

	double RXThresh_; // receive power threshold (W)
	double CSThresh_; // carrier sense threshold (W)
	double CPThresh_; // capture threshold (dB)

	Antenna *ant_;
	Propagation *propagation_; // Propagation Model
	Modulation *modulation_; // Modulation Scheme

	enum ChannelStatus {
		SLEEP, IDLE, RECV, SEND
	};

	bool isInterfaceOn_; // on-off status of this node
	WirelessPhyInterfaceSleepTimer sleep_timer_;
	int status_;

private:
	inline int initialized() {
		return (node_ && uptarget_ && downtarget_ && propagation_);
	}
	void updateIdleEnergy();
	void updateSleepEnergy();

	// Convenience method
	EnergyModel* em() {
		return node()->energy_model();
	}
};

#endif /* __wireless_phy_interface_h__ */

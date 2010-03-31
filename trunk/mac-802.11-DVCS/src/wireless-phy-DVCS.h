#ifndef __wireless_phy_DVCS_h__
#define __wireless_phy_DVCS_h__

#include <wireless-phy.h>

class WirelessPhyDVCS: public WirelessPhy {
public:
    WirelessPhyDVCS();

    inline double getAntennaX() {
        return ant_->getX();
    }
    inline double getAntennaY() {
        return ant_->getY();
    }

    inline int getBoresight() const {
        return boresight_;
    }
    inline void setBoresight(int boresight) {
        boresight_ = boresight_;
    }

protected:
    int boresight_; // antenna boresight;
};

#endif /* __wireless_phy_DVCS_h__ */

#include <math.h>

#include <packet.h>

#include <mobilenode.h>
#include <phy.h>
#include <propagation.h>
#include <modulation.h>
#include <omni-antenna.h>
#include <packet.h>
#include <ip.h>
#include <agent.h>
#include <trace.h>
#include <sys/param.h>  /* for MIN/MAX */

#include <diffusion/diff_header.h>

#include "wireless-phy-DVCS.h"

static class WirelessPhyDVCSClass: public TclClass {
public:
    WirelessPhyDVCSClass() :
        TclClass("Phy/WirelessPhy/DVCS") {
    }

    TclObject* create(int, const char* const *) {
        return (new WirelessPhyDVCS);
    }
} class_WirelessPhyDVCS;

WirelessPhyDVCS::WirelessPhyDVCS() :
    WirelessPhy() {
    boresight_ = 0;
}

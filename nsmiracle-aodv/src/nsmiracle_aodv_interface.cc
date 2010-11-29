#include <ip.h>

#include "nsmiracle_aodv_interface.h"
#include "nsmiracle_aodv_packet.h"

#define MAODV_INTF_NEXT_HOP_UNREACHABLE_DEPHT  5
#define MAODV_INTF_NEXT_HOP_UNREACHABLE_REASON "NHU"

static class MAODVIPInterfaceModuleClass: public TclClass {
public:
    MAODVIPInterfaceModuleClass() :
        TclClass("Module/IP/Interface/MAODV") {
    }

    TclObject* create(int, const char* const *) {
        return (new MAODVIPInterfaceModule());

    }
} class_MAODVIPInterfaceModule;

MAODVIPInterfaceModule::MAODVIPInterfaceModule() {
}

MAODVIPInterfaceModule::~MAODVIPInterfaceModule() {
}

int MAODVIPInterfaceModule::command(int argc, const char* const * argv) {
    return IPModule::command(argc, argv);
}

void MAODVIPInterfaceModule::recv(Packet *p) {
    recv(p, 0);
}

void MAODVIPInterfaceModule::recv(Packet *p, int id) {
    hdr_ip *iph = HDR_IP(p);
    hdr_cmn *ch = HDR_CMN(p);
    struct hdr_maodv *ah = HDR_MAODV(p);

    if (ch->direction() == hdr_cmn::UP) { /* direction UP */
        // If this node is either the next hop or the dest, send the packet UP
        if ((ch->next_hop_ == ipAddr_) || (iph->daddr() == ipAddr_)
                || (iph->daddr() == IP_BROADCAST)) {
            fprintf(
                    stderr,
                    "INTF %d: Receiving packet AODV type %d|%d from %d (%d) next hop %d destination %d\n",
                    ipAddr_, ah->ah_type, ch->ptype(), iph->saddr(),
                    ch->prev_hop_, ch->next_hop_, iph->daddr());
            sendUp(p); //
        } else { // Otherwise, drop the packet
            drop(p, MAODV_INTF_NOT_FOR_ME_DEPTH, MAODV_INTF_NOT_FOR_ME_REASON);
        }
    } else { /* direction DOWN */
        //If    next hop can be reached via this interface, send the packet DOWN
        if ((iph->daddr() == IP_BROADCAST) || ((ch->next_hop_ & subnet_)
                == (ipAddr_ & subnet_))) {
            fprintf(
                    stderr,
                    "INTF %d: Sending packet AODV type %d|%d destined for %d (%d) next hop %d\n",
                    ipAddr_, ah->ah_type, ch->ptype(), iph->daddr(),
                    ch->prev_hop_, ch->next_hop_);
            sendDown(p);
        } else { // Otherwise, drop the packet
            fprintf(stderr, "INTF %d: Dropping packet destined for %d (%d)\n",
                    ipAddr_, iph->daddr(), ch->next_hop_);
            drop(p, MAODV_INTF_NEXT_HOP_UNREACHABLE_DEPHT,
                    MAODV_INTF_NEXT_HOP_UNREACHABLE_REASON);
        }
    }
}

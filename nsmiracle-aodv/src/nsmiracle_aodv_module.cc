#include "nsmiracle_aodv_module.h"

static class MAODVModuleClass: public TclClass {
public:
    MAODVModuleClass() :
        TclClass("Module/IP/Routing/MAODV") {
    }

    TclObject* create(int, const char* const *) {
        return (new MAODVModule());
    }
} class_MAODVModule;

MAODVModule::MAODVModule() {
}

MAODVModule::~MAODVModule() {
}

int MAODVModule::command(int argc, const char* const * argv) {
    Tcl& tcl = Tcl::instance();
    if (argc == 2) {
        if (strcasecmp(argv[1], "get-MAODV-agent") == 0) {
            if (maodvAgent_) {
                tcl.result(maodvAgent_->name());
            } else {
                /*
                 * This block is intended to be executed
                 * only by Module/Transport/TCP init instproc
                 * see tcp-transport-modules.tcl for details
                 */
                tcl.resultf("%s", "NONE");
            }
            return TCL_OK;
        }
    } else if (argc == 3) {
        if (strcasecmp(argv[1], "set-MAODV-agent") == 0) {
            maodvAgent_ = dynamic_cast<MAODV*> (TclObject::lookup(argv[2]));

            if (!maodvAgent_) {
                return TCL_ERROR;
            }

            tcl.evalf("%s target %s", maodvAgent_->name(), name());
            return TCL_OK;
        }
    }

    return IPModule::command(argc, argv);
}

/**
 * This method is called by MAODV::send()
 * since standard ns AODVCA see MAODVModule
 * as being their (down)target. MAODVModule then
 * forwards the packet to lower layers using sendDown()
 *
 *
 * @param p pointer to the packet to be sent
 * @param callback unused by all known MAODVModule, kept only for
 *                 compatibility
 */
void MAODVModule::recv(Packet* p, Handler* callback) {
    sendDown(p);
}

void MAODVModule::recv(Packet *p) {
    recv(p, 0);
}

void MAODVModule::recv(Packet *p, int id) {
    assert(maodvAgent_ != NULL);

    hdr_ip *iph = HDR_IP(p);
    hdr_cmn *ch = HDR_CMN(p);
    fprintf(stderr,
            "Module %d: Receiving packet from %d (%d) next hop %d destination %d\n",
            getId(), iph->saddr(), ch->prev_hop_, ch->next_hop_, iph->daddr());

    maodvAgent_->recv(p, NULL);
}

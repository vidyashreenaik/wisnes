#ifndef __nsmiracle_aodv_interface_h__
#define __nsmiracle_aodv_interface_h__

#include <ipmodule.h>

#define MAODV_INTF_NOT_FOR_ME_DEPTH 5
#define MAODV_INTF_NOT_FOR_ME_REASON "NFM"

class MAODVIPInterfaceModule: public IPModule {
public:
    MAODVIPInterfaceModule();
    virtual ~MAODVIPInterfaceModule();

    virtual void recv(Packet *p);
    virtual void recv(Packet *packet, int id);
    virtual int command(int argc, const char* const * argv);
};

#endif  /* __nsmiracle_aodv_interface_h__ */

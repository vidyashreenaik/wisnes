#ifndef __nsmiracle_aodv_module_h__
#define __nsmiracle_aodv_module_h__

#include <ipmodule.h>

#include "nsmiracle_aodv.h"

class MAODVModule: public IPModule {
public:
    MAODVModule();
    virtual ~MAODVModule();

    virtual void recv(Packet* p, Handler* callback);
    virtual void recv(Packet *packet);
    virtual void recv(Packet *packet, int id);
    virtual int command(int argc, const char* const * argv);

protected:
    MAODV* maodvAgent_;
};

#endif /* __nsmiracle_aodv_module_h__ */

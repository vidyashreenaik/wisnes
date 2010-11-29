#include <tclcl.h>

extern EmbeddedTcl Mac_dvcsTclCode;

extern "C" int Mac_dvcs_Init() {
    Mac_dvcsTclCode.load();

    return 0;
}

extern "C" int Cygmac_dvcs_Init() {
    Mac_dvcs_Init();
}


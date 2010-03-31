#include <tclcl.h>

extern EmbeddedTcl Mac80211DVCSTclCode;

extern "C" int Mac80211DVCS_Init()
{ 
  Mac80211DVCSTclCode.load();
  
  return 0;
}

extern "C" int  Cygmac80211DVCS_Init()
{
  Mac80211DVCS_Init();
}


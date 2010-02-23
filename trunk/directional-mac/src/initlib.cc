#include <tclcl.h>

extern EmbeddedTcl Directional_macTclCode;

extern "C" int Directional_mac_Init()
{ 
  Directional_macTclCode.load();
  
  return 0;
}

extern "C" int  Cygdirectional_mac_Init()
{
  Directional_mac_Init();
}


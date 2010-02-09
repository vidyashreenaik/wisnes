#include <tclcl.h>

extern EmbeddedTcl Directional_antennaTclCode;

extern "C" int Directional_antenna_Init()
{ 
  Directional_antennaTclCode.load();
  
  return 0;
}

extern "C" int  Cygdirectional_antenna_Init()
{
  Directional_antenna_Init();
}


#include <tclcl.h>

extern EmbeddedTcl Ns_tools_extensionsTclCode;

extern "C" int Ns_tools_extensions_Init()
{ 
  Ns_tools_extensionsTclCode.load();
  
  return 0;
}

extern "C" int  Cygns_tools_extensions_Init()
{
  Ns_tools_extensions_Init();
}


static char code[] = "Module/IP/Routing/MAODV set debug_ 0\n\
\n\
Module/IP/Routing/MAODV instproc init {args} {\n\
eval $self next $args\n\
}\n\
\n\
proc createMAODVModule {ipAddr} {\n\
set ragent [new Agent/MAODV $ipAddr]\n\
[Simulator instance] at 0.0 \"$ragent start\"  ; # Start BEACON/HELLO Message\n\
\n\
set module [new Module/IP/Routing/MAODV]\n\
$module set-MAODV-agent $ragent\n\
\n\
return $module\n\
}\n\
\n\
Module/IP/Interface/MAODV set debug_ 0\n\
\n\
\n\
PacketHeaderManager set tab_(PacketHeader/MAODV)         1\n\
";
#include "tclcl.h"
EmbeddedTcl NsmiracleaodvTclCode(code);

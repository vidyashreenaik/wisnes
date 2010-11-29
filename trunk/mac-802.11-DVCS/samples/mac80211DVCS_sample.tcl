source dynlibutils.tcl

dynlibload directional_antenna
dynlibload mac_dvcs         ../src/.libs

set val(chan)   Channel/WirelessChannel             ;# channel type
set val(prop)   Propagation/TwoRayGround            ;# radio-propagation model
set val(netif)  Phy/WirelessPhy                     ;# network interface type
set val(mac)    Mac/802_11/DVCS                     ;# MAC type
set val(ifq)    Queue/DropTail/PriQueue             ;# interface queue type
set val(ll)     LL                                  ;# link layer type
set val(ant)    Antenna/DirectionalAntenna/FlatTop  ;# antenna model
#set val(ant)    Antenna/OmniAntenna                 ;# antenna model
set val(ifqlen) 50                                  ;# max packet in ifq
set val(nn)     4                                   ;# number of mobilenodes
set val(rp)     AODV                                ;# routing protocol
set val(x)		500
set val(y)		500

# Initialize Global Variables
set ns_		[new Simulator]
set tracefd     [open directional_antenna_sample.tr w]
$ns_ trace-all $tracefd

set namtrace [open directional_antenna_sample.nam w]
$ns_ namtrace-all-wireless $namtrace $val(x) $val(y)

Antenna/DirectionalAntenna/FlatTop set Debug_ 1
Antenna/DirectionalAntenna/FlatTop set Beamwidth_ 60
# set up topography object
set topo       [new Topography]

$topo load_flatgrid $val(x) $val(y)

# Create God
create-god $val(nn)

# Create channe
set chan_ [new $val(chan)]

# configure node, please note the change below.
$ns_ node-config -adhocRouting $val(rp) \
		-llType $val(ll) \
		-macType $val(mac) \
		-ifqType $val(ifq) \
		-ifqLen $val(ifqlen) \
		-antType $val(ant) \
		-propType $val(prop) \
		-phyType $val(netif) \
		-topoInstance $topo \
		-agentTrace ON \
		-routerTrace ON \
		-macTrace ON \
		-movementTrace OFF \
		-channel $chan_

Antenna/DirectionalAntenna/FlatTop set Boresight_ 0.0
set node_(0) [$ns_ node]
Antenna/DirectionalAntenna/FlatTop set Boresight_ 180.0
set node_(1) [$ns_ node]
Antenna/DirectionalAntenna/FlatTop set Boresight_ 0.0
set node_(2) [$ns_ node]
#$ns_ node-config -antType Antenna/OmniAntenna
Antenna/DirectionalAntenna/FlatTop set Boresight_ 180.0
set node_(3) [$ns_ node]

$node_(0) random-motion 0
$node_(1) random-motion 0
$node_(2) random-motion 0
$node_(3) random-motion 0

$node_(0) set X_ 50.0
$node_(0) set Y_ 500.0
$node_(0) set Z_ 0.0

$node_(1) set X_ 200.0
$node_(1) set Y_ 500.0
$node_(1) set Z_ 0.0

$node_(2) set X_ 50.0
$node_(2) set Y_ 450.0
$node_(2) set Z_ 0.0

$node_(3) set X_ 200.0
$node_(3) set Y_ 450.0
$node_(3) set Z_ 0.0

for {set i 0} {$i < $val(nn)} {incr i} {
	$ns_ initial_node_pos $node_($i) 20
}

#
# Setup for directional transmission
#
for {set i 0} {$i < $val(nn)} {incr i} {
    set antenna [$node_($i) get-antenna 0]
    set mac [$node_($i) get-mac 0]

    $antenna set Id_ $i
    $mac directional-antenna $antenna
}

#
# Check antenna directions
#

#set antenna_(0) [$node_(0) get-antenna 0]
#set antenna_(1) [$node_(1) get-antenna 0]

#puts [format "Ant 1: Dir %3.2f | Ant 2: dir %3.2f" [$antenna_(0) set Boresight_] [$antenna_(1) set Boresight_]]

#$antenna_(0) set Boresight_ 90.0
#$antenna_(1) set Boresight_ 270.0

#set antenna_(0) [$node_(0) get-antenna 0]
#set antenna_(1) [$node_(1) get-antenna 0]

#puts [format "Ant 1: Dir %3.2f | Ant 2: dir %3.2f" [$antenna_(0) set Boresight_] [$antenna_(1) set Boresight_]]
#
# Provide initial (X,Y, for now Z=0) co-ordinates for mobilenodes
#

set tcp [new Agent/TCP]
$tcp set class_ 2
set sink [new Agent/TCPSink]
$ns_ attach-agent $node_(0) $tcp
$ns_ attach-agent $node_(1) $sink
$ns_ connect $tcp $sink
set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ns_ at 3.0 "$ftp start"

set tcp2 [new Agent/TCP]
$tcp2 set class_ 2
set sink2 [new Agent/TCPSink]
$ns_ attach-agent $node_(3) $tcp2
$ns_ attach-agent $node_(2) $sink2
$ns_ connect $tcp2 $sink2
set ftp2 [new Application/FTP]
$ftp2 attach-agent $tcp2
$ns_ at 5.0 "$ftp2 start"

#
# Tell nodes when the simulation ends
#
proc stop {} {
    global ns_ tracefd
    $ns_ flush-trace
    close $tracefd
}

for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at 90.0 "$node_($i) reset";
}
$ns_ at 90.0 "stop"
$ns_ at 90.01 "puts \"NS EXITING...\" ; $ns_ halt"

puts "Starting Simulation..."
$ns_ run

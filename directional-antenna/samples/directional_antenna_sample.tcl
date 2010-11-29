source dynlibutils.tcl

dynlibload directional_antenna     ../src/.libs

set val(chan)   Channel/WirelessChannel             ;# channel type
set val(prop)   Propagation/TwoRayGround            ;# radio-propagation model
set val(netif)  Phy/WirelessPhy                     ;# network interface type
set val(mac)    Mac/802_11                          ;# MAC type
set val(ifq)    Queue/DropTail/PriQueue             ;# interface queue type
set val(ll)     LL                                  ;# link layer type
set val(ant)    Antenna/DirectionalAntenna/FlatTop  ;# antenna model
set val(ifqlen) 50                                  ;# max packet in ifq
set val(nn)     2                                   ;# number of mobilenodes
set val(rp)     AODV                                ;# routing protocol
set val(x)		300
set val(y)		300

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

Antenna/DirectionalAntenna/FlatTop set Boresight_ 90.0
Antenna/DirectionalAntenna/FlatTop set Id_ 0
set node_(0) [$ns_ node]
Antenna/DirectionalAntenna/FlatTop set Id_ 1
Antenna/DirectionalAntenna/FlatTop set Boresight_ 270.0
set node_(1) [$ns_ node]

$node_(0) random-motion 0
$node_(1) random-motion 0

for {set i 0} {$i < $val(nn)} {incr i} {
	$ns_ initial_node_pos $node_($i) 20
}

#
# Check antenna directions
#

set antenna_(0) [$node_(0) getAntenna 0]
set antenna_(1) [$node_(1) getAntenna 0]

puts [format "Ant 1: Dir %3.2f | Ant 2: dir %3.2f" [$antenna_(0) set Boresight_] [$antenna_(1) set Boresight_]]

#$antenna_(0) set Boresight_ 90.0
#$antenna_(1) set Boresight_ 270.0

#set antenna_(0) [$node_(0) getAntenna 0]
#set antenna_(1) [$node_(1) getAntenna 0]

#puts [format "Ant 1: Dir %3.2f | Ant 2: dir %3.2f" [$antenna_(0) set Boresight_] [$antenna_(1) set Boresight_]]
#
# Provide initial (X,Y, for now Z=0) co-ordinates for mobilenodes
#
$node_(0) set X_ 50.0
$node_(0) set Y_ 50.0
$node_(0) set Z_ 0.0

$node_(1) set X_ 50.0
$node_(1) set Y_ 200.0
$node_(1) set Z_ 0.0

set tcp [new Agent/TCP]
$tcp set class_ 2
set sink [new Agent/TCPSink]
$ns_ attach-agent $node_(0) $tcp
$ns_ attach-agent $node_(1) $sink
$ns_ connect $tcp $sink
set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ns_ at 3.0 "$ftp start"

#
# Tell nodes when the simulation ends
#
proc stop {} {
    global ns_ tracefd
    $ns_ flush-trace
    close $tracefd
}

for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at 30.0 "$node_($i) reset";
}
$ns_ at 30.0 "stop"
$ns_ at 30.01 "puts \"NS EXITING...\" ; $ns_ halt"

puts "Starting Simulation..."
$ns_ run


source dynlibutils.tcl

dynlibload directional_antenna     ../src/.libs

set val(chan)           Channel/WirelessChannel             ;# channel type
set val(prop)           Propagation/TwoRayGround            ;# radio-propagation model
set val(netif)          Phy/WirelessPhy                     ;# network interface type
set val(mac)            Mac/802_11                          ;# MAC type
set val(ifq)            Queue/DropTail/PriQueue             ;# interface queue type
set val(ll)             LL                                  ;# link layer type
set val(ant)            Antenna/DirectionalAntenna/Practical;# antenna model
set val(ifqlen)         50                                  ;# max packet in ifq
set val(nn)             2                                   ;# number of mobilenodes
set val(rp)             AODV                                ;# routing protocol
set val(x)		        500
set val(y)		        500

Antenna/DirectionalAntenna/Practical set Debug_ 1
# Initialize Global Variables
set ns_		[new Simulator]
set tracefd     [open directional_antenna_sample.tr w]
$ns_ trace-all $tracefd

set namtrace [open directional_antenna_sample.nam w]
$ns_ namtrace-all-wireless $namtrace $val(x) $val(y)

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

set node_(0) [$ns_ node]
set node_(1) [$ns_ node]

$node_(0) random-motion 0
Antenna/DirectionalAntenna/Practical set Azimuth_ 180
$node_(1) random-motion 0

for {set i 0} {$i < $val(nn)} {incr i} {
	$ns_ initial_node_pos $node_($i) 20
}

#
# Provide initial (X,Y, for now Z=0) co-ordinates for mobilenodes
#
$node_(0) set X_ 50.0
$node_(0) set Y_ 50.0
$node_(0) set Z_ 0.0

$node_(1) set X_ 150.0
$node_(1) set Y_ 50.0
$node_(1) set Z_ 0.0

#
# Now produce some simple node movements
# Node_(1) starts to move towards node_(0)
#
#$ns_ at 3.0 "$node_(1) setdest 50.0 40.0 25.0"
#$ns_ at 3.0 "$node_(0) setdest 48.0 38.0 5.0"

# Node_(1) then starts to move away from node_(0)
#$ns_ at 20.0 "$node_(1) setdest 490.0 480.0 30.0" 

# Setup traffic flow between nodes
# TCP connections between node_(0) and node_(1)

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


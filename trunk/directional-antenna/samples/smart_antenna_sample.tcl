source dynlibutils.tcl

dynlibload smart_antenna     ../src/.libs

set val(chan)           Channel/WirelessChannel             ;# channel type
set val(prop)           Propagation/TwoRayGround            ;# radio-propagation model
set val(netif)          Phy/WirelessPhy                     ;# network interface type
set val(mac)            Mac/802_11                          ;# MAC type
set val(ifq)            Queue/DropTail/PriQueue             ;# interface queue type
set val(ll)             LL                                  ;# link layer type
set val(ant)            Antenna/SmartAntenna                ;# antenna model
set val(ifqlen)         50                                  ;# max packet in ifq
set val(nn)             2                                   ;# number of mobilenodes
set val(rp)             AODV                                ;# routing protocol
set val(x)		        500
set val(y)		        500

proc record  {} {
    global ns_ thrtrace sink
    
    set thrValue ""

    # Set the time after which the procedure should be called again
    set time 0.5
    # How many bytes have been received by the traffic sinks?
    set bw [$sink set bytes_]
    # Get the current time
    set now [$ns_ now]
    # Calculate the bandwidth (in MBit/s) and write it to the files
    puts $thrtrace "$now\t[expr $bw/$time*8/1000000]"
    # Reset the bytes_ values on the traffic sinks
    $sink set bytes_ 0
    # Re-schedule the procedure
    $ns_ at [expr $now+$time] "record"
}

# Initialize Global Variables
set ns_		[new Simulator]
$ns_ use-newtrace

set tracefd     [open smart_antenna_sample.tr w]
$ns_ trace-all $tracefd

set namtrace [open smart_antenna_sample.nam w]
$ns_ namtrace-all-wireless $namtrace $val(x) $val(y)

set thrtrace [open smart_antenna_sample.thr w]

# set up topography object
set topo       [new Topography]

$topo load_flatgrid $val(x) $val(y)

# Create God
create-god $val(nn)

# Create channel
set chan_ [new $val(chan)]

# Configurations
Agent/TCP set packetSize_ 900

Antenna/SmartAntenna set Debug_ 1

# Configure node
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
# Repoint the second node towards first node
Antenna/SmartAntenna set Boresight_ 180
set node_(1) [$ns_ node]

$node_(0) random-motion 0
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

set tcp [new Agent/TCP]
$tcp set class_ 2
set sink [new Agent/TCPSink]
$ns_ attach-agent $node_(0) $tcp
$ns_ attach-agent $node_(1) $sink
$ns_ connect $tcp $sink
set ftp [new Application/FTP]
$ftp attach-agent $tcp
$ns_ at 3.0 "$ftp start" 


$ns_ at 0.5 "record"
#
# Tell nodes when the simulation ends
#
proc stop {} {
    global ns_ tracefd namtrace thrtrace
    $ns_ flush-trace
    close $tracefd
    close $namtrace
    close $thrtrace
}

for {set i 0} {$i < $val(nn) } {incr i} {
    $ns_ at 30.0 "$node_($i) reset";
}
$ns_ at 30.0 "stop"
$ns_ at 30.01 "puts \"NS EXITING...\" ; $ns_ halt"

puts "Starting Simulation..."
$ns_ run


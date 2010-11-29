################################################################################
# Command-line parameters
################################################################################


if {$argc == 0} {
    # default parameters if none passed on the command line
    set opt(seed)   100
    set opt(run)    1
} elseif {$argc != 2} {
    puts " invalid argc ($argc)"
    puts " usage: $argv0 seed run"
    exit
} else {
    set opt(seed)   [lindex $argv 0]
    set opt(run)    [lindex $argv 1]
}

################################################################################
# Scenario Configuration
################################################################################


set opt(nn) 4
set opt(x)  300
set opt(y)  300
set opt(duration)   155            ;# Duration of each transmission
set opt(startmin)   5              ;# Minimum starting time of each transmission
set opt(startmax)   10             ;# Maximum starting time of each transmission

set opt(chan)       Channel/WirelessChannel  ;# Channel type
#set opt(chan)       Channel/WirelessChannel/PowerAware  ;# Channel type
set opt(prop)       Propagation/FreeSpace    ;# Radio-propagation
#set opt(prop)       Propagation/FreeSpace/PowerAware    ;# Radio-propagation
set opt(netif)      Phy/WirelessPhy          ;# Network interface type
#set opt(netif)      Phy/WirelessPhy/PowerAware          ;# Network interface type
set opt(mac)        Mac/802_11                ;# MAC type
#set opt(mac)        Mac/802_11/Multirate                ;# MAC type
set opt(ifq)        Queue/DropTail/PriQueue             ;# Interface queue type
set opt(ll)         LL                                  ;# Link layer type
set opt(ant)        Antenna/OmniAntenna                 ;# Antenna model
set opt(ifqlen)     50                                  ;# Max packet in ifq
set opt(rp)         AODV                                ;# Routing protocol

set opt(tracefname) "simple_omni_antenna_demo.tr"
set opt(namfname)   "simple_omni_antenna_demo.nam"
set opt(thrfname)   "simple_omni_antenna_demo.thr"
set opt(perfname)   "simple_omni_antenna_demo.per"
set opt(fttfname)   "simple_omni_antenna_demo.ftt"

################################################################################
# Module Libraries
################################################################################


# The following lines must be before loading libraries
# and before instantiating the ns Simulator
#
# remove-all-packet-headers

source dynlibutils.tcl
dynlibload dei80211mr

################################################################################
# Procedures
################################################################################


proc finish {} {
    global opt ns tf hf pf ff

    puts " DONE."

    $ns flush-trace
    close $tf
    close $hf
    close $pf
    close $ff
    
    puts "Tracefile: $opt(tracefname)"
    puts "  Thrfile: $opt(thrfname)"
    puts "  Perfile: $opt(perfname)"
    puts "  Fttfile: $opt(fttfname)"
}

proc recordStats  {} {
    global opt ns hf pf ff meshTcpSinks
    
    set perValue ""
    set fttValue ""
    set thrValue ""

    # Set the time after which the procedure should be called again
    set time 0.5
    # How many bytes have been received by the traffic sinks?
    set bw0 [$meshTcpSinks(0) set bytes_]
    set bw1 [$meshTcpSinks(1) set bytes_]
    # Get the current time
    set now [$ns now]
    # Calculate the bandwidth (in MBit/s) and write it to the files
    set perValue ""
    set fttValue ""
    set thrValue "$now\t[expr $bw0/$time*8/1000000]\t[expr $bw1/$time*8/1000000]"
    puts $hf $thrValue
    puts $pf $perValue
 	puts $ff $fttValue
    # Reset the bytes_ values on the traffic sinks
    $meshTcpSinks(0) set bytes_ 0
    $meshTcpSinks(1) set bytes_ 0
    # Re-schedule the procedure
    $ns at [expr $now+$time] "recordStats"
}

proc createMeshNode {n xPos yPos} {
    global opt ns per \
           meshNodes meshMacs meshIfqs \
           meshPhys

    puts "Creating mesh node $n"
    set meshNodes($n)  [$ns node]

#    $meshNodes($n) setPowerProfile 0 [new PowerProfile]

#    set meshMacs($n) [$meshNodes($n) getMac 0]
#    set meshIfqs($n) [$meshNodes($n) getIfq 0]
#    set meshPhys($n) [$meshNodes($n) getPhy 0]

#    $meshMacs($n) dataMode_ Mode11Mb
#    $meshMacs($n) basicMode_ Mode1Mb
#    $meshMacs($n) nodes $opt(nn)
#    $meshPhys($n) set CSTresh_ 7.7e-11 

#    $meshNodes($n) setPER 0 $per

    $meshNodes($n) set X_ $xPos
    $meshNodes($n) set Y_ $yPos
    $meshNodes($n) set Z_ 0
    $meshNodes($n) random-motion 0
}

################################################################################
# Simulator instance
################################################################################


set ns [new Simulator]
$ns use-newtrace

################################################################################
# Random Number Generators
################################################################################


global defaultRNG
$defaultRNG seed 100

set startrng [new RNG]

set rvstart [new RandomVariable/Uniform]
$rvstart set min_ $opt(startmin)
$rvstart set max_ $opt(startmax)
$rvstart use-rng $startrng

# seed random number generator according to replication number
for {set j 1} {$j < $opt(run)} {incr j} {
    $defaultRNG next-substream
    $startrng next-substream
}

################################################################################
# Override Default Configuration
################################################################################


# New procedures
Node/MobileNode instproc getIfq { param0} {
    $self instvar ifq_    
    return $ifq_($param0) 
}

Node/MobileNode instproc getPhy { param0} {
    $self instvar netif_    
    return $netif_($param0) 
}

# TCP
Agent/TCP set packetSize_ 1500

# Queue
Queue/DropTail/PriQueue set Prefer_Routing_Protocols    1
Queue/DropTail/PriQueue set size_ 1000

# 802.11b MAC
Mac/802_11 set dataRate_        11Mb            ;# Rate for data frames
Mac/802_11 set basicRate_       1Mb             ;# Rate for control frames
Mac/802_11 set RTSThreshold_    2000            ;# RTS/CTS exchange for packets
                                                ;# bigger than 2000 bytes only
                                                ;# (0 means always use RTS/CTS)
Mac/802_11 set ShortRetryLimit_ 7               ;# Retransmittions
Mac/802_11 set LongRetryLimit_  4               ;# Retransmissions
#Mac/802_11/Multirate set useShortPreamble_ true
#Mac/802_11/Multirate set gSyncInterval_ 0.000005
#Mac/802_11/Multirate set bSyncInterval_ 0.00001

Phy/WirelessPhy set bandwidth_  11Mb            ;# Data rate
Phy/WirelessPhy set freq_       2.437e9         ;# Channel 13 (2.472GHz)
Phy/WirelessPhy set L_          1.0             ;# System loss factor
# 802.11b Channel (OriNOCO11b Card)
Phy/WirelessPhy set Pt_         3.1622777e-2    ;# Transmit Power (15dBm)
Phy/WirelessPhy set CPThresh_   10              ;# Collision Threshold
Phy/WirelessPhy set CSThresh_   7.7e-11         ;# Carrier Sense Power = Around
                                                ;# Receiver Sensivity for BPSK -
                                                ;# 10 dBm
Phy/WirelessPhy set RXThresh_   1.15126e-10     ;# Receiver Power Threshold =
                                                ;# Receiver Sensivity - 1 dBm
Phy/WirelessPhy set debug_ 0
Channel/WirelessChannel set debug_ 0
#Phy/WirelessPhy/PowerAware set debug_ 0

#Channel/WirelessChannel/PowerAware set distInterference_ 200


################################################################################
# Global allocations
################################################################################


set tf [open $opt(tracefname) w]
$ns trace-all $tf
set nf [open $opt(namfname) w]
$ns namtrace-all-wireless $nf $opt(x) $opt(y)

set hf [open $opt(thrfname) w]
set pf [open $opt(perfname) w]
set ff [open $opt(fttfname) w]

set topo      [new Topography]
$topo load_flatgrid $opt(x) $opt(y)

set god       [create-god $opt(nn)]

#set noisePower 7e-11
#set per       [new PER]
#$per set noise_ $noisePower
#$per loadPERTable80211bIntersilHFA3861B        ;# Load 802.11b PER values table

#set peerstats [new PeerStatsDB/Static]
#$peerstats numpeers [expr $opt(nn) + 1]

# Create channel
set chan_ [new $opt(chan)]

# Configure node
$ns node-config -adhocRouting $opt(rp) \
		-llType $opt(ll) \
		-macType $opt(mac) \
		-ifqType $opt(ifq) \
		-ifqLen $opt(ifqlen) \
		-antType $opt(ant) \
		-propType $opt(prop) \
		-phyType $opt(netif) \
		-topoInstance $topo \
		-agentTrace ON \
		-routerTrace ON \
		-macTrace ON \
		-movementTrace OFF \
		-channel $chan_ 

################################################################################
#  Create nodes 
################################################################################


createMeshNode 0    0      0
createMeshNode 1    0      90
createMeshNode 2    90    50
createMeshNode 3    180    50

set meshTcps(0)     [new Agent/TCP/Reno ]
$meshTcps(0) set fid_ 1
$ns attach-agent $meshNodes(0) $meshTcps(0)
set meshFtps(0) [new Application/FTP]
$meshFtps(0) attach-agent $meshTcps(0)
set meshTcpSinks(0) [new Agent/TCPSink]
$ns attach-agent $meshNodes(1) $meshTcpSinks(0)
$ns connect $meshTcps(0) $meshTcpSinks(0)
set meshFtpStart(0) [$rvstart value]
set meshFtpStop(0)  [expr $meshFtpStart(0) + $opt(duration)]
$ns at $meshFtpStart(0) "$meshFtps(0) start"
$ns at $meshFtpStop(0)  "$meshFtps(0) stop"

set meshTcps(1)     [new Agent/TCP/Reno ]
$meshTcps(1) set fid_ 2
$ns attach-agent $meshNodes(2) $meshTcps(1)
set meshFtps(1) [new Application/FTP]
$meshFtps(1) attach-agent $meshTcps(1)
set meshTcpSinks(1) [new Agent/TCPSink]
$ns attach-agent $meshNodes(3) $meshTcpSinks(1)
$ns connect $meshTcps(1) $meshTcpSinks(1)
set meshFtpStart(1) [$rvstart value]
set meshFtpStop(1)  [expr $meshFtpStart(1) + $opt(duration)]
$ns at $meshFtpStart(1) "$meshFtps(1) start"
$ns at $meshFtpStop(1)  "$meshFtps(1) stop"

################################################################################
#  Start Simulation
################################################################################


puts -nonewline "SIMULATING "
for {set i 1} {$i < 50} {incr i} {
    $ns at [expr $opt(startmax) + (($opt(duration) * $i)/ 50.0) ] \
           "puts -nonewline . ; flush stdout"
}
$ns at $opt(startmin) "recordStats"
$ns at [expr $opt(startmax) + $opt(duration)] "finish; $ns halt"

$ns run


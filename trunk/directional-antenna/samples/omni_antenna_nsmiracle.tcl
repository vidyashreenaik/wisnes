################################################################################
# Command-line parameters
################################################################################

if {$argc == 0} {
    # default parameters if none passed on the command line
    set opt(npr)    2
    set opt(gindex) 1
} elseif {$argc != 2} {
    puts " invalid argc ($argc)"
    puts " usage: $argv0 nodes-per-row gateway-node-index"
    exit
} else {
    set opt(npr)    [lindex $argv 0]
    set opt(gindex) [lindex $argv 1]
}

################################################################################
# Scenario Configuration
################################################################################


set opt(nn)     [expr $opt(npr)*$opt(npr)]
set opt(ndist)  100
set opt(xstart) 100
set opt(ystart) 100
set opt(xdist)  [expr $opt(xstart)*2 + $opt(ndist)*$opt(npr)]
set opt(ydist)  [expr $opt(ystart)*2 + $opt(ndist)*$opt(npr)]
set opt(seed)   100
set opt(run)    5
# duration of each transmission
set opt(duration)   155
# starting time of each transmission
set opt(startmin)   5
set opt(startmax)   10
set opt(tracefname) "smart_antenna_nsmiracle_$opt(npr).tr"
set opt(thrfname)   "smart_antenna_nsmiracle_$opt(npr).thr"
set opt(perfname)   "smart_antenna_nsmiracle_$opt(npr).per"
set opt(fttfname)   "smart_antenna_nsmiracle_$opt(npr).ftt"

################################################################################
# Module Libraries
################################################################################


# The following lines must be before loading libraries
# and before instantiating the ns Simulator
#
# remove-all-packet-headers

source dynlibutils.tcl
dynlibload Miracle
dynlibload miraclelink
dynlibload MiracleBasicMovement
dynlibload MiracleWirelessCh
dynlibload MiraclePhy802_11
dynlibload MiracleMac802_11
dynlibload miracleport
dynlibload miraclecbr
dynlibload cbrtracer
dynlibload aodvuu
dynlibload Trace
dynlibload aodvuutracer
dynlibload dei80211mr
dynlibload smart_antenna            ../src/.libs
dynlibload phytracer

################################################################################
# Procedures
################################################################################


proc finish {} {
    global opt ns tf hf pf ff wrdCbrs

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
    global opt ns hf pf ff wrdCbrs

    set now [$ns now]
    set perValue "$now"
    set fttValue "$now"
    set thrValue "$now"
    for {set i 0} {$i < $opt(npr)} {incr i} {
        set perValue "$perValue\t[$wrdCbrs(0,$i) getper]"
        set fttValue "$fttValue\t[$wrdCbrs(0,$i) getftt]"
        set thrValue "$thrValue\t[$wrdCbrs(0,$i) getthr]"
    }
    puts $hf $thrValue
    puts $pf $perValue
 	puts $ff $fttValue

	$ns at [expr $now + 2] "recordStats"
}

proc createMeshNode {n ipAddr ipSubnet xPos yPos} {
    global ns pmodel per wirelessChannel \
           meshNodes meshCbrs meshPorts meshCbrPorts meshAodvs meshIpifs \
           meshLls meshPhys meshPps meshRas meshPositions

    puts "Creating mesh node $n"

    set meshNodes($n)   [$ns create-M_Node]
    set meshCbrs($n)    [new Module/CBR]
    set meshPorts($n)   [new Module/Port/Map]
    set meshAodvs($n)   [new Module/AODVUU]
    set meshIpifs($n,0) [new Module/IP/AODVInterface]


    $meshCbrs($n) set period_ 0.05

    $meshIpifs($n,0)      addr $ipAddr
    $meshIpifs($n,0)      subnet $ipSubnet

    set meshLls($n)     [ \
                            create802_11MacModule "LL/Mrcl" \
                            "Queue/DropTail/PriQueue" "Mac/802_11/Multirate" \
                            [$meshIpifs($n,0) addr] "" 100 \
                        ]
    set meshPhys($n)    [ \
                            createPhyModule "Phy/WirelessPhy/PowerAware" \
                            $pmodel "Antenna/OmniAntenna" $meshLls($n) "" \
                        ]
    
    set mac            [$meshLls($n) getMac]
    set phy            [$meshPhys($n) getPhy]
    $mac               basicMode_ Mode1Mb
    $mac               dataMode_ Mode11Mb
    $mac               per $per
    set meshPps($n)    [new PowerProfile]
    $mac               powerProfile $meshPps($n)
    $phy               powerProfile $meshPps($n)

    set meshRas($n)    [new RateAdapter/ARF]
    $meshRas($n)       attach2mac $mac
    $meshRas($n)       use80211b
    $meshRas($n)       setmodeatindex 0

    # Add modules
    $meshNodes($n) addModule 6 $meshCbrs($n)    3 "n${n}_CBR"
    $meshNodes($n) addModule 5 $meshPorts($n)   3 "n${n}_PORT"
    $meshNodes($n) addModule 4 $meshAodvs($n)   3 "n${n}_AODV"
    $meshNodes($n) addModule 3 $meshIpifs($n,0) 3 "n${n}_IP"
    $meshNodes($n) addModule 2 $meshLls($n)     3 "n${n}_LL"
    $meshNodes($n) addModule 1 $meshPhys($n)    3 "n${n}_PHY"

    #Connect modules
    $meshNodes($n) setConnection $meshCbrs($n)      $meshPorts($n)   1
    $meshNodes($n) setConnection $meshPorts($n)     $meshAodvs($n)   1
    $meshNodes($n) setConnection $meshAodvs($n)     $meshIpifs($n,0) 1
    $meshNodes($n) setConnection $meshIpifs($n,0)   $meshLls($n)     1
    $meshNodes($n) setConnection $meshLls($n)       $meshPhys($n)

    #Add to channel
    $meshNodes($n) addToChannel $wirelessChannel $meshPhys($n) 1

    $meshAodvs($n) add-if $meshIpifs($n,0)
    $meshAodvs($n) if-queue [$meshLls($n) getQueue]
    
    #Set position
    set meshPositions($n) [new "Position/BM"]
    $meshNodes($n) addPosition $meshPositions($n)
    $meshPositions($n) setX_ $xPos
    $meshPositions($n) setY_ $yPos
    
    set meshCbrPorts($n) [$meshPorts($n) assignPort $meshCbrs($n)]
}

proc addGatewayInterface {n ipAddr ipSubnet defaultGw} {
    global meshNodes meshAodvs meshIpifs
    
    puts "Adding gateway interface to mesh node $n"
    
    set meshIpifs($n,1)     [new Module/IP/AODVInterface]

    $meshIpifs($n,1)        addr $ipAddr
    $meshIpifs($n,1)        subnet $ipSubnet

    # Add modules
    $meshNodes($n) addModule 3 $meshIpifs($n,1) 3 "n${n}_IP1"

    # Connect modules
    $meshNodes($n) setConnection $meshAodvs($n)  $meshIpifs($n,1) 1

    $meshAodvs($n) add-gwif $meshIpifs($n,1)
    $meshAodvs($n) set internet_gw_mode_ 1
    $meshAodvs($n) set-default-gw $defaultGw

    $meshAodvs($n) set internet_gw_mode_ 1
}

proc createWiredNode {n ipAddr ipSubnet cbrCount} {
    global ns wrdNodes wrdCbrs wrdPorts wrdCbrPorts wrdIpifs
    puts "Creating wired node $n with $cbrCount CBR receivers "
    
    set wrdNodes($n)    [$ns create-M_Node]
    set wrdPorts($n)    [new Module/Port/Map]
    set wrdIpifs($n)    [new Module/IP]

    $wrdIpifs($n)       addr $ipAddr
    $wrdIpifs($n)       subnet $ipSubnet

    $wrdNodes($n) addModule 2 $wrdPorts($n)   3 "wrd_PORT"
    $wrdNodes($n) addModule 1 $wrdIpifs($n)   3 "wrd_IPIF"
    
    $wrdNodes($n) setConnection $wrdPorts($n) $wrdIpifs($n) 1
    
    for {set i 0} {$i < $cbrCount} {incr i} {
        set wrdCbrs($n,$i)     [new Module/CBR]
        
        # Add modules
        $wrdNodes($n) addModule 3 $wrdCbrs($n,$i) 3 "wrd_CBR$i"


        # Connect modules
        $wrdNodes($n) setConnection $wrdCbrs($n,$i)  $wrdPorts($n) 1
        
        set wrdCbrPorts($n,$i) [$wrdPorts($n) assignPort $wrdCbrs($n,$i)]
    }
}

################################################################################
# Simulator instance
################################################################################


set ns [new Simulator]
$ns use-Miracle

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
# Override Default Module Configuration
################################################################################


# 802.11b MAC
Mac/802_11 set dataRate_        11Mb            ;# Rate for data frames
Mac/802_11 set basicRate_       1Mb             ;# Rate for control frames
Mac/802_11 set RTSThreshold_    2000            ;# RTS/CTS exchange for packets
                                                ;# bigger than 2000 bytes only
                                                ;# (0 means always use RTS/CTS)
Mac/802_11 set ShortRetryLimit_ 7               ;# Retransmittions
Mac/802_11 set LongRetryLimit_  4               ;# Retransmissions

Mac/802_11/Multirate set useShortPreamble_ true ;# Short preamble
Mac/802_11/Multirate set gSyncInterval_ 0.000005
Mac/802_11/Multirate set bSyncInterval_ 0.00001

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

Queue/DropTail/PriQueue set Prefer_Routing_Protocols    1
Queue/DropTail/PriQueue set size_ 1000

ConnectorTrace/ChSAP set debug_ 0
ConnectorTrace/Bin set debug_ 0

ConnectorTrace/Bin set depth 5

Phy/WirelessPhy/PowerAware set debug_ 0

Module/IP set debug_ 0

Module/AODVUU set llfeedback_ 1
Module/AODVUU set debug_ 0


################################################################################
# Global allocations
################################################################################


set tf [open $opt(tracefname) w]
$ns trace-all $tf

set hf [open $opt(thrfname) w]
set pf [open $opt(perfname) w]
set ff [open $opt(fttfname) w]

set pmodel [new Propagation/MrclFreeSpace]
set wirelessChannel [new Module/DumbWirelessCh]
$wirelessChannel setTag "CHA"

# Create the link between sink node and base station i.e Internet
set duplexlink [new Module/DuplexLink]
$duplexlink bandwidth 100Mb
$duplexlink delay 0.001
$duplexlink qsize 50
$duplexlink settags "INT"

set topo      [new Topography]
$topo load_flatgrid [expr $opt(xdist) + 1] [expr $opt(ydist) + 1]

set god       [create-god [expr $opt(nn) + 1]]

set noisePower 7e-11
set per       [new PER]
$per set noise_ $noisePower
$per loadPERTable80211bIntersilHFA3861B        ;# Load 802.11b PER values table

set peerstats [new PeerStatsDB/Static]
$peerstats numpeers [expr $opt(nn) + 1]

################################################################################
#  Create nodes 
################################################################################


for {set i 0} {$i < $opt(npr)} {incr i} {
    for {set j 0} {$j < $opt(npr)} {incr j} {
        set nodeIndex  [expr $i*$opt(npr)+$j]
        set nodePosY   [expr $opt(ystart) + $opt(ndist)*$i]
        set nodePosX   [expr $opt(xstart) + $opt(ndist)*$j]
        set nodeIpAddr 10.0.0.[expr $nodeIndex+1]
        
        createMeshNode $nodeIndex $nodeIpAddr 255.255.255.0 $nodePosX $nodePosY
    }
}
addGatewayInterface $opt(gindex) 192.168.0.1 255.255.255.0 192.168.0.1
createWiredNode 0 192.168.0.2 255.255.255.0 7

# create link between sink node and gateway
$duplexlink connect $meshNodes($opt(gindex)) $meshIpifs($opt(gindex),1) 1 \
                    $wrdNodes(0) $wrdIpifs(0) 1

# connect and start CBRs
for {set i 0} {$i < $opt(npr)} {incr i} {
    $meshCbrs($i)  set destAddr_ [$wrdIpifs(0) addr]
    $meshCbrs($i)  set destPort_ $wrdCbrPorts(0,$i)
    $wrdCbrs(0,$i) set destAddr_ [$meshIpifs($i,0) addr]
    $wrdCbrs(0,$i) set destPort_ $meshCbrPorts($i)
    
    set meshCbrsStart($i) [$rvstart value]
    set meshCbrsStop($i)  [expr $meshCbrsStart($i) + $opt(duration)]
    $ns at $meshCbrsStart($i) "$meshCbrs($i) start"
    $ns at $meshCbrsStop($i) "$meshCbrs($i) stop"
}

################################################################################
#  Start Simulation
################################################################################


puts -nonewline "SIMULATING "
for {set i 0} {$i < $opt(nn)} {incr i} {
    $ns at 0 "$meshAodvs($i) start"
}
for {set i 1} {$i < 50} {incr i} {
    $ns at [expr $opt(startmax) + (($opt(duration) * $i)/ 50.0) ] \
           "puts -nonewline . ; flush stdout"
}
$ns at $opt(startmin) "recordStats"
$ns at [expr $opt(startmax) + $opt(duration)] "finish; $ns halt"

$ns run


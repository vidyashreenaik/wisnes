################################################################################
# Command-line parameters
################################################################################

if {$argc == 0} {
    set opt(run) 1
    set opt(nn) 4
    set opt(gateway) 3
    set opt(duration) 60
    set opt(sampling) 1.0
} elseif {$argc != 5 } {
    puts "usage: ns $argv0 replicationnumber numnodes gatewaynodeindex \
            duration samplinginterval"
    exit
} else {
    set opt(run)      [lindex $argv 0]
    set opt(nn)       [lindex $argv 1]       
    set opt(gateway)  [lindex $argv 2]
    set opt(duration) [lindex $argv 3]
    set opt(sampling) [lindex $argv 4]
}


################################################################################
# Scenario Configuration
################################################################################

set opt(fixedNodeId) 1

set opt(xmax)   110
set opt(xmin)   30

# starting time of each transmission
set opt(startmin)     1
set opt(startmax)     1.05

set opt(resultfname) "nsmiracle-isa-sample.log"
set opt(tracefname)  "nsmiracle-isa-sample.tr"
set opt(thrfname)    "nsmiracle-isa-sample.thr"
set opt(perfname)    "nsmiracle-isa-sample.per"
set opt(fttfname)    "nsmiracle-isa-sample.ftt"

################################################################################
# Module Libraries
################################################################################

# The following lines must be before loading libraries
# and before instantiating the ns Simulator
#
remove-all-packet-headers

source dynlibutils.tcl

dynlibload Miracle
dynlibload MiracleIp
dynlibload MiracleIpRouting
dynlibload miracletcp
dynlibload miracleport
dynlibload miraclelink
dynlibload MiracleWirelessCh
dynlibload MiraclePhy802_11
dynlibload MiracleMac802_11
dynlibload dei80211mr
dynlibload aodvuu
#dynlibload nsmiracleisa ../src/.libs

################################################################################
# Tracers
################################################################################

#dynlibload tcptracer
#dynlibload routingtracer
#dynlibload ClTrace
#dynlibload multiratetracer
#dynlibload phytracer
#dynlibload aodvuutracer


################################################################################
# NS-2 Instance & Trace Files
################################################################################

set ns [new Simulator]
$ns use-Miracle

set tf [open $opt(tracefname) w]
$ns trace-all $tf

set hf [open $opt(thrfname) w]
set pf [open $opt(perfname) w]
set ff [open $opt(fttfname) w]

################################################################################
# Random Number Generators
################################################################################

global defaultRNG
set startrng [new RNG]
set positionrng [new RNG]

set rvstart [new RandomVariable/Uniform]
$rvstart set min_ $opt(startmin)
$rvstart set max_ $opt(startmax)
$rvstart use-rng $startrng

set rvposition [new RandomVariable/Uniform]
$rvposition set min_ 0
$rvposition set max_ $opt(xmax)
$rvposition use-rng $positionrng

# Seed random number generator according to replication number
for {set j 1} {$j < $opt(run)} {incr j} {
    $defaultRNG next-substream
    $startrng next-substream
    $positionrng next-substream
}


################################################################################
# Functions
################################################################################

proc createMeshNode {id ip netmask posX posY} {

# +------------------------------------------------------------------------+
# | 	                         5. Routing                                |
# +------------------------------------------------------------------------+
# | 	                        4.1 IP Interface                           |
# +------------------------------------------------------------------------+
# | 	                3. Interface Selection Layer                       |
# +-----------------+-----------------+-----------------+------------------+
# |2.1 Module/802.11|2.2 Module/802.11|2.3 Module/802.11| 2.3 Module/802.11|
# +-----------------+-----------------+-----------------+------------------+
# | 	                        1. Module/Phy  	                           |
# +-----------------------------------+------------------------------------+
#                                     |           
# +-----------------------------------+------------------------------------+
# |                          DumbWirelessChannel                           |
# +------------------------------------------------------------------------+

    global ns channel per pmodel peerstats meshNode meshTcp meshPort \
           meshTcpPortNum meshTcpAgent meshIpr meshIpif meshMac meshPhy \
           meshMrMac
    
    set meshNode($id) [$ns create-M_Node]     
    set meshTcp($id)  [new Module/TCP/Reno]
    set meshPort($id) [new Module/Port]
    set meshIpif($id) [new Module/IP/AODVInterface]
    set meshIpr($id)  [new Module/AODVUU]

    # need this in advance for ARP
    $meshIpif($id) addr   $ip
    $meshIpif($id) subnet $netmask
    
    set meshMac($id)  [ \
                      create802_11MacModule "LL/Mrcl" \
                      "Queue/DropTail/PriQueue" "Mac/802_11/Multirate" \
                      [$meshIpif($id) addr] "" 100
                      ]
    set meshPhy($id)  [ \
                      createPhyModule "Phy/WirelessPhy/PowerAware" $pmodel \
                      "Antenna/OmniAntenna" $meshMac($id) "" \
                      ]

    set mac [$meshMac($id)  getMac]
    set phy [$meshPhy($id)  getPhy]

    $mac  basicMode_ Mode6Mb
    $mac  dataMode_ Mode6Mb
    $mac  per $per
    #$phy  PeerStatsDB $peerstats
    set meshPp($id)  [new PowerProfile]
    $mac  powerProfile $meshPp($id)
    $phy  powerProfile $meshPp($id)
    #$phy  set freq_ [$channel set freq_]

    set meshTcpAgent($id) [$meshTcp($id) getTcpAgent]

    $meshNode($id) addModule 6 $meshTcp($id)  1 "MTCP "
    $meshNode($id) addModule 5 $meshPort($id) 1 "MPRT "
    $meshNode($id) addModule 4 $meshIpr($id)  1 "MIPR "
    $meshNode($id) addModule 3 $meshIpif($id) 1 "MIPF "
    $meshNode($id) addModule 2 $meshMac($id)  1 "MMAC "
    $meshNode($id) addModule 1 $meshPhy($id)  1 "MPHY "

    $meshNode($id) setConnection $meshTcp($id)  $meshPort($id)  0
    $meshNode($id) setConnection $meshPort($id) $meshIpr($id)   0
    $meshNode($id) setConnection $meshIpr($id)  $meshIpif($id)  0
    $meshNode($id) setConnection $meshIpif($id) $meshMac($id)   0
    $meshNode($id) setConnection $meshMac($id)  $meshPhy($id)   0
    $meshNode($id) addToChannel  $channel       $meshPhy($id)   0

    $meshIpr($id) add-if $meshIpif($id)
    $meshIpr($id) if-queue [$meshMac($id) getQueue]

    # assign TCP ports
    set meshTcpPortNum($id) [$meshPort($id) assignPort $meshTcp($id)]

    # setup meshNode position
    set meshPos($id) [new "Position/BM"]
    $meshNode($id)  addPosition $meshPos($id)
    $meshPos($id)   setX_ $posX
    $meshPos($id)   setY_ $posY
    
    puts [format "Mesh Node $id : Address $ip/$netmask : X = %2f, Y = %2f" \
            $posX $posY]
}

proc attachFtpSourceToMeshNode {meshNodeId sourceId sourceStart} {

    global ns opt meshFtp meshTcpAgent
    
    set meshFtp($sourceId)    [new Application/FTP]
    $meshFtp($sourceId) attach-agent $meshTcpAgent($meshNodeId)
    
    $ns at $sourceStart "$meshFtp($sourceId) start"
    $ns at [expr $sourceStart + $opt(duration)]  "$meshFtp($sourceId) stop"
}

proc addGatewayToMeshNode {meshNodeId gatewayId ip netmask defaultGw} {

# +------------------------------------------------------------------------+
# | 	                     4.2 IP Interface                              |
# +------------------------------------------------------------------------+

    global ns meshNode meshIpr meshGateway

    set meshGateway($gatewayId)    [new Module/IP/AODVInterface]

    $meshGateway($gatewayId) addr   $ip
    $meshGateway($gatewayId) subnet $netmask

    $meshNode($meshNodeId) addModule 3 $meshGateway($gatewayId)   0 "MGIF "

    $meshNode($meshNodeId) setConnection $meshIpr($meshNodeId) \
                                         $meshGateway($gatewayId)  1

    $meshIpr($meshNodeId) add-gwif $meshGateway($gatewayId)
    $meshIpr($meshNodeId) set internet_gw_mode_ 1
	$meshIpr($meshNodeId) set-default-gw $defaultGw
}

proc createFixedNode {id ip netmask defaultGw} {

# +------------------------------------------------------------------------+
# | 	                          6. PORT                                  |
# +------------------------------------------------------------------------+
# | 	                      5. IP Routing                                |
# +------------------------------------------------------------------------+
# | 	                     4. IP Interface                               |
# +------------------------------------------------------------------------+

    global ns fixedNode fixedPort fixedIpr fixedIpif
    
    set fixedNode($id)   [$ns create-M_Node] 
    set fixedIpif($id)   [new Module/IP/Interface]
    set fixedIpr($id)    [new Module/IP/Routing]
    set fixedPort($id)   [new Module/Port]

    $fixedIpif($id) addr   $ip
    $fixedIpif($id) subnet $netmask

    $fixedIpr($id) defaultGateway $defaultGw

    $fixedNode($id) addModule 4  $fixedIpif($id) 0 "FIPF  "
    $fixedNode($id) addModule 5  $fixedIpr($id)  0 "FIPR  "
    $fixedNode($id) addModule 6  $fixedPort($id) 0 "FPRT  "

    $fixedNode($id) setConnection $fixedPort($id) $fixedIpr($id)   1
    $fixedNode($id) setConnection $fixedIpr($id)  $fixedIpif($id)  1
}

proc attachSinkToFixedNode {fixedNodeId sinkId} {

# +------------------------------------------------------------------------+
# | 	                          7. Sink                                  |
# +------------------------------------------------------------------------+

    global ns fixedNode fixedPort fixedTcpSink fixedTcpSinkAgent fixedTcpPortNum
    
    set fixedTcpSink($sinkId)       [new Module/TCPSink] 
    set fixedTcpSinkAgent($sinkId)  [$fixedTcpSink($sinkId) getTcpAgent]
    
    $fixedNode($fixedNodeId) addModule 7   $fixedTcpSink($sinkId) 0 \
                                                "FTCPS$sinkId"
    $fixedNode($fixedNodeId) setConnection $fixedTcpSink($sinkId) \
                                                $fixedPort($fixedNodeId) 1
    
    set fixedTcpPortNum($sinkId) [$fixedPort($fixedNodeId) assignPort \
                                                    $fixedTcpSink($sinkId)]
}

proc connectMeshAgentToFixedAgent {agentId nodeId} {
    global meshTcpAgent meshTcpPortNum meshIpif fixedTcpSinkAgent \
           fixedTcpPortNum fixedIpif
    
    $fixedTcpSinkAgent($agentId) set agent_addr_ [$fixedIpif($nodeId) addr]
    $fixedTcpSinkAgent($agentId) set dst_addr_ [$meshIpif($agentId) addr]
    $fixedTcpSinkAgent($agentId) set agent_port_ $fixedTcpPortNum($agentId)
    $fixedTcpSinkAgent($agentId) set dst_port_ $meshTcpPortNum($agentId)

    $meshTcpAgent($agentId) set agent_addr_  [$meshIpif($agentId) addr]
    $meshTcpAgent($agentId) set dst_addr_  [$fixedIpif($nodeId) addr]
    $meshTcpAgent($agentId) set agent_port_  $meshTcpPortNum($agentId)
    $meshTcpAgent($agentId) set dst_port_  $fixedTcpPortNum($agentId)
}

proc recordStats {} {
    global opt ns hf pf ff fixedTcpSinkAgent
    
    set perValue ""
    set fttValue ""
    set thrValue ""

    # How many bytes have been received by the traffic sinks?
    for {set id 1} {$id <= $opt(nn)} {incr id} {
        set sinkBytes($id) [$fixedTcpSinkAgent($id) set bytes_]
    }
    # Get the current time
    set now [$ns now]
    # Calculate the bandwidth (in MBit/s) and write it to the files
    set perValue ""
    set fttValue ""
    set thrValue "$now"
    for {set id 1} {$id <= $opt(nn)} {incr id} {
        set bandwidth [format "%.5f" \
                            [expr $sinkBytes($id)/$opt(sampling)*8/1000000]]
        set thrValue "$thrValue\t$bandwidth"
    }
    
    puts $hf $thrValue
    puts $pf $perValue
 	puts $ff $fttValue
    # Reset the bytes_ values on the traffic sinks
    for {set id 1} {$id <= $opt(nn)} {incr id} {
        $fixedTcpSinkAgent($id) set bytes_ 0
    }

    # Re-schedule the procedure
    $ns at [expr $now + $opt(sampling)] "recordStats"
}

proc finishSimulation {} {
    global ns tf opt meshTcpAgent
    puts "done!"
    $ns flush-trace
    close $tf

    puts ""
    puts "Tracefile: $opt(tracefname)"
    puts "  Thrfile: $opt(thrfname)"
    puts "  Perfile: $opt(perfname)"
    puts "  Fttfile: $opt(fttfname)"
}


################################################################################
# Override Default Module Configuration
################################################################################

PeerStatsDB/Static set debug_ 0

Agent/TCP set packetSize_     1000

Mac/802_11 set RTSThreshold_ 2000
Mac/802_11 set ShortRetryLimit_ 3
Mac/802_11 set LongRetryLimit_ 3
Mac/802_11/Multirate set useShortPreamble_ true
Mac/802_11/Multirate set gSyncInterval_ 0.000005
Mac/802_11/Multirate set bSyncInterval_ 0.00001

Phy/WirelessPhy set Pt_ 0.01
Phy/WirelessPhy set freq_ 2437e6
Phy/WirelessPhy set L_ 1.0
Queue/DropTail/PriQueue set Prefer_Routing_Protocols    1
Queue/DropTail/PriQueue set size_ 1000

set noisePower 7e-11
Phy/WirelessPhy set CSTresh_ [expr $noisePower * 1.1]

# starting TCP window
Agent/TCP set windowInit_ 
Agent/TCP set ts_resetRTO_ 1
Agent/TCP set timestamps_ 1


################################################################################
# Global allocations
################################################################################

set channel [new Module/DumbWirelessCh]
$channel setTag "MCHA "

set pmodel [new Propagation/MrclFreeSpace]
create-god [expr $opt(nn) + 1]

set per [new PER]
$per set noise_ $noisePower
$per loadDefaultPERTable

set peerstats [new PeerStatsDB/Static]
$peerstats numpeers [expr $opt(nn) + 1]


################################################################################
#  Create Fixed Node
################################################################################
createFixedNode $opt(fixedNodeId) "2.0.0.2" "255.255.0.0" "2.0.0.1"


################################################################################
#  Create Mesh Nodes
################################################################################

for {set id 1} {$id <= $opt(nn)} {incr id} {
    set posX [$rvposition value]
    set posY [$rvposition value]

    createMeshNode $id "1.0.0.$id" "255.255.0.0" $posX $posY
    # For each meshNode, a TCPSink is needed at the fixedNode
    attachSinkToFixedNode $opt(fixedNodeId) $id
    connectMeshAgentToFixedAgent $id $opt(fixedNodeId)
}


################################################################################
#  Create FTP Applications
################################################################################

for {set id 1} {$id <= $opt(nn)} {incr id} {
    attachFtpSourceToMeshNode $id $id [$rvstart value]
}


################################################################################
#  Add Gateway
################################################################################

addGatewayToMeshNode $opt(gateway) 1 "2.0.0.1" "255.255.0.0" "2.0.0.2"


################################################################################
#  Create The Internet!!!
################################################################################

set internet [new Module/DuplexLink]
$internet delay      0.1
$internet bandwidth  10000000
$internet qsize      10
$internet settags "INT  "

$internet connect $meshNode($opt(gateway)) $meshGateway(1) 0 $fixedNode(1) \
                  $fixedIpif(1) 0


################################################################################
#  Start Simulation
################################################################################

for {set id 1} {$id <= $opt(nn)} {incr id} {
    $ns at 0.0 "$meshIpr($id) start"
}
$ns at 0.0 "recordStats"
$ns at [expr $opt(startmax) + $opt(duration) + 5] "finishSimulation; $ns halt"

puts -nonewline "Simulating..."
$ns run


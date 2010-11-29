source dynlibutils.tcl

dynlibload hwmp     ../src/.libs

set MeshEdge 175.0
set MeshNumberOfEdge 3

# ======================================================================
# Define options
# ======================================================================

set opt(chan)               Channel/WirelessChannel
set opt(prop)               Propagation/TwoRayGround
set opt(netif)              Phy/WirelessPhy
set opt(mac)                Mac/802_11
set opt(ifq)                Queue/DropTail/PriQueue
set opt(ll)                 LL
set opt(ant)                Antenna/OmniAntenna
set opt(x)                  [expr ($MeshEdge*($MeshNumberOfEdge+2))]
set opt(y)                  [expr ($MeshEdge*($MeshNumberOfEdge+2))]
set opt(ifqlen)             50
set opt(tr)                 wired-cum-wireless-sim-hwmp.tr
set opt(namtr)              wired-cum-wireless-sim-hwmp.nam
set opt(nn)                 [expr $MeshNumberOfEdge*$MeshNumberOfEdge]
set opt(adhocRouting)       HWMP
set opt(rtAgentFunction)    create-hwmp-agent
set opt(stop)               250
set num_wired_nodes         3
set num_bs_nodes            1


set ns_ [new Simulator]
# flow colors
$ns_ color 1 red
$ns_ color 2 blue
$ns_ color 3 green
# set up for hierarchical routing
$ns_ node-config -addressType hierarchical
AddrParams set domain_num_ 2
lappend cluster_num 1 2             
AddrParams set cluster_num_ $cluster_num
lappend eilastlevel 100 1 2
AddrParams set nodes_num_ $eilastlevel 

# set up tracing
$ns_ use-newtrace
set tracefd  [open $opt(tr) w]
$ns_ trace-all $tracefd
set namtracefd [open $opt(namtr) w]
$ns_ namtrace-all $namtracefd

set topo   [new Topography]
$topo load_flatgrid $opt(x) $opt(y)
# god needs to know the number of all wireless interfaces
create-god [expr $opt(nn) + $num_bs_nodes]

# create wired nodes
set addresses {1.0.0 1.1.0 1.1.1}
for {set i 0} {$i < $num_wired_nodes} {incr i} {
    set W($i) [$ns_ node [lindex $addresses $i]]
}

# configuration for base station
$ns_ node-config -rtAgentFunction $opt(rtAgentFunction) \
             -adhocRouting $opt(adhocRouting) \
             -llType $opt(ll) \
             -macType $opt(mac) \
             -ifqType $opt(ifq) \
             -ifqLen $opt(ifqlen) \
             -antType $opt(ant) \
             -propInstance [new $opt(prop)] \
             -phyType $opt(netif) \
             -channel [new $opt(chan)] \
             -topoInstance $topo \
             -wiredRouting ON \
             -agentTrace ON \
             -routerTrace ON \
             -macTrace OFF

# create base station
set base_station_ [$ns_ node 0.0.0]
$base_station_ random-motion 0
$base_station_ set X_ 0.0
$base_station_ set Y_ 0.0
$base_station_ set Z_ 0.0
$ns_ initial_node_pos $base_station_ 20

# configuration for mobilenodes
$ns_ node-config -wiredRouting OFF

# the following strings organize a square mesh network:
for {set row 0} {$row < $MeshNumberOfEdge } {incr row} {
	for {set column 0} {$column < $MeshNumberOfEdge} {incr column} {
		set node_id [expr ($row*$MeshNumberOfEdge+$column)]
        set node_($node_id) [$ns_ node 0.0.[expr $node_id+1]]
        $node_($node_id) base-station [AddrParams addr2id [$base_station_ node-addr]]
		$node_($node_id) random-motion 0
        $node_($node_id) set X_ [expr $MeshEdge*($column+1)]
		$node_($node_id) set Y_ [expr $MeshEdge*($row+1)]
		$node_($node_id) set Z_ 0.0
        $ns_ initial_node_pos $node_($node_id) 20
	}
}

# create links between wired and BS nodes
$ns_ duplex-link $W(2) $W(0) 1Mb 2ms DropTail
$ns_ duplex-link $W(1) $W(0) 1Mb 2ms DropTail
$ns_ duplex-link $W(0) $base_station_ 5Mb 2ms DropTail
$ns_ duplex-link-op $W(2) $W(0) orient right-down
$ns_ duplex-link-op $W(1) $W(0) orient right-up
$ns_ duplex-link-op $W(0) $base_station_ orient right

# Internet-bound traffic
set traffic_source {6}
set traffic_sink {0}
for {set i 0} {$i < [llength $traffic_source]} {incr i} {
    set tcp($i) [new Agent/TCP]
    $tcp($i) set class_ 2
    set sink($i) [new Agent/TCPSink]
    $ns_ attach-agent $node_([lindex $traffic_source $i]) $tcp($i)
    $ns_ attach-agent $W([lindex $traffic_sink $i]) $sink($i)
    $ns_ connect $tcp($i) $sink($i)
    set ftp($i) [new Application/FTP]
    $ftp($i) attach-agent $tcp($i)
    $ns_ at 60 "$ftp($i) start"
}

# Intra-mesh traffic
set traffic_source {1 2}
set traffic_sink {4 5}
for {set i 0} {$i < [llength $traffic_source]} {incr i} {
    set tcp($i) [new Agent/TCP]
    $tcp($i) set class_ 2
    set sink($i) [new Agent/TCPSink]
    $ns_ attach-agent $node_([lindex $traffic_source $i]) $tcp($i)
    $ns_ attach-agent $node_([lindex $traffic_sink $i]) $sink($i)
    $ns_ connect $tcp($i) $sink($i)
    set ftp($i) [new Application/FTP]
    $ftp($i) attach-agent $tcp($i)
    #$ns_ at 60 "$ftp($i) start"
}

for {set i } {$i < $opt(nn) } {incr i} {
  $ns_ at $opt(stop).0000010 "$node_($i) reset";
}
$ns_ at $opt(stop).0000010 "$base_station_ reset";

$ns_ at $opt(stop).1 "puts \"NS EXITING...\"; $ns_ halt"

puts "Starting Simulation..."
$ns_ run

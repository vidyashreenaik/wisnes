source dynlibutils.tcl

dynlibload Miracle
dynlibload Trace
#dynlibload ClTrace
dynlibload MiracleIp
dynlibload MiracleIpRouting
#dynlibload MiracleSandbox
dynlibload miraclecbr
dynlibload MiracleBasicMovement

dynlibload MiracleWirelessCh
dynlibload MiraclePhy802_11
dynlibload MiracleMac802_11

dynlibload miracleport

dynlibload dei80211mr
dynlibload multiratetracer
dynlibload phytracer

dynlibload nsmiracleaodv ../src/.libs

set tracefname "nsmiracle-aodv-sample.tr"

Phy/WirelessPhy/PowerAware set debug_ 0

proc finish {} {
    global ns tf tracefname datafname
    puts "---> SIMULATION DONE."
    $ns flush-trace
    close $tf
    puts "Tracefile: $tracefname"
}

set macmr(0) 0

proc createNode {application title n} {
#   -----------------
#   | 6. Application|
#   -----------------
#   | 5. Transport  |
#   -----------------
#   |   4. AODVCA   |
#   -----------------
#   |   3. IP-IF    |
#   -----------------
#   |   2. LL-CA    |
#   -----------------
#   |     1. PHY    |
#   -----------------
#
    global ns channel pmodel per macmr ip port

    set node [$ns create-M_Node]
    set transport [new Module/Port]

    set IPIF [new Module/IP/Interface/MAODV]
    $IPIF addr "10.0.0.${n}"
    $IPIF subnet "255.255.0.0"
    set LL [create802_11MacModule "LL/Mrcl" "Queue/DropTail/PriQueue" "Mac/802_11/Multirate" [$IPIF addr] "" 100 ]
    set AODVCA [createMAODVModule [$IPIF addr]]
    set PHY [createPhyModule "Phy/WirelessPhy/PowerAware" $pmodel "Antenna/OmniAntenna" $LL ""]


    set mac [$LL getMac]
    set phy [$PHY getPhy]
    $mac basicMode_ Mode6Mb
    $mac dataMode_ Mode6Mb
    $mac per $per
    set pp [new PowerProfile]
    $mac powerProfile $pp
    $phy powerProfile $pp


    set ra [new RateAdapter/ARF]
    $ra attach2mac $mac
    $ra use80211g
    $ra setmodeatindex 0

    set position [new "Position/BM"]

    $node addPosition $position

    $node addModule 6 $application 3 "APP"
    $node addModule 5 $transport 3 "TRANS"
    $node addModule 4 $AODVCA 3 "AODVCA"
    $node addModule 3 $IPIF 3 "IPIF"
    $node addModule 2 $LL 3 "LL"
    $node addModule 1 $PHY 3 "PHY"
    $node setConnection $application $transport 1
    $node setConnection $transport $AODVCA 1
    $node setConnection $AODVCA $IPIF 1
    $node setConnection $IPIF $LL 1
    $node setConnection $LL $PHY 1
    $node addToChannel $channel $PHY 1

    set port [$transport assignPort $application]
    puts "$title"
    puts "Application id [$application id_]"
    puts "Transport id [$transport id_]"
    puts "AODVCA id [$AODVCA id_]"
    puts "IPIF id [$IPIF id_] address [$IPIF addr]"
    puts "LL id [$LL id_]"
    puts "PHY id [$PHY id_]"
    puts ""
    set ip [$IPIF addr]
    set macmr([$IPIF addr]) $mac
    return $node
}

Module/IP/Interface set debug_ 0

set per [new PER]
$per loadDefaultPERTable
$per set noise_ 7e-11

Mac/802_11 set RTSThreshold_ 2000
Mac/802_11 set ShortRetryLimit_ 4
Mac/802_11 set LongRetryLimit_ 3
Mac/802_11/Multirate set useShortPreamble_ true
Mac/802_11/Multirate set gSyncInterval_ 0.000005
Mac/802_11/Multirate set bSyncInterval_ 0.00001

Phy/WirelessPhy set Pt_ 0.01
Phy/WirelessPhy set freq_ 2437e6
Phy/WirelessPhy set L_ 1.0
Queue/DropTail/PriQueue set Prefer_Routing_Protocols    1
Queue/DropTail/PriQueue set size_ 1000

ConnectorTrace/ChSAP set debug_ 0
ConnectorTrace/Bin set debug_ 0

ConnectorTrace/Bin set depth 5

Mac/802_11 set RTSThreshold_ 2000

set ns [new Simulator]
$ns use-Miracle

set tf [open $tracefname w]
$ns trace-all $tf

set pmodel [new Propagation/MrclTwoRayGround]
#set pmodel [new Propagation/MrclFreeSpace]

#set channel [createWirelessChModule "" "" ""]
set channel [new Module/DumbWirelessCh]


create-god 2

puts "---> CREATING NODE0"

set sender   [new Module/CBR]
set node1 [createNode $sender "Sender Node" 1]
set position1 [$node1 position]
$position1 setX_ 0.0
$position1 setY_ 0.0

puts "---> CREATING NODE1"

set receiver [new Module/CBR]
set node2 [createNode $receiver "Receiver Node" 2]
set position2 [$node2 position]
$position2 setX_ 0.0
$position2 setY_ 0.0

$sender set destPort_ $port
$sender set destAddr_ $ip

puts "Noise: [expr 10 * log10([$per set noise_]) + 30] dBm"

puts "---> BEGIN SIMULATION"


#for {set i 1} {$i < 20} {incr i} {
#    $ns at $i "puts [$macmr(0) getSnr 1]"
#}

$ns at 0 "$sender start"
$ns at 1 "$position2 setdest 0 100 10"
$ns at 12 "$sender stop"
$ns at 13 "finish; $ns halt"
$ns run

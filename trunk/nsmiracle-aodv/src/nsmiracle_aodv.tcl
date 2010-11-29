Module/IP/Routing/MAODV set debug_ 0

Module/IP/Routing/MAODV instproc init {args} {
    eval $self next $args
}

proc createMAODVModule {ipAddr} {
    set ragent [new Agent/MAODV $ipAddr]
    [Simulator instance] at 0.0 "$ragent start"  ; # Start BEACON/HELLO Message

    set module [new Module/IP/Routing/MAODV]
    $module set-MAODV-agent $ragent

	return $module
}

Module/IP/Interface/MAODV set debug_ 0

# Packet Headers needed by the TCP modules wrapped in Miracle modules

PacketHeaderManager set tab_(PacketHeader/MAODV)         1

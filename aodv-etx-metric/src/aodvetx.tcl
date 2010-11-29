#PacketHeaderManager set tab_(PacketHeader/Flags)       1
#PacketHeaderManager set tab_(PacketHeader/Mac)         1
#PacketHeaderManager set tab_(PacketHeader/IP)          1
PacketHeaderManager set tab_(PacketHeader/AODVETX)     1

Agent/AODVETX  set debug_ 0

Agent/AODVETX  set sport_ 0
Agent/AODVETX  set dport_ 0

Agent/AODVETX instproc init args {
    $self next $args
}

Simulator instproc create-aodvetx-agent { node } {
    set ragent [new Agent/AODVETX [$node node-addr]]
    $self at 0.0 "$ragent start"               ; # Start BEACON/HELLO Message
    $node set ragent_ $ragent

    return $ragent
}

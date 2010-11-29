PacketHeaderManager set tab_(PacketHeader/AODVRTT)     1

Agent/AODVRTT  set debug_ 0

Agent/AODVRTT  set sport_ 0
Agent/AODVRTT  set dport_ 0

Agent/AODVRTT instproc init args {
    $self next $args
}

Simulator instproc create-aodvrtt-agent { node } {
    set ragent [new Agent/AODVRTT [$node node-addr]]
    $self at 0.0 "$ragent start"               ; # Start BEACON/HELLO Message
    $node set ragent_ $ragent

    return $ragent
}

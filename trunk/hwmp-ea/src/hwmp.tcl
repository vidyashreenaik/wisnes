#PacketHeaderManager set tab_(PacketHeader/Flags)      1
#PacketHeaderManager set tab_(PacketHeader/Mac)        1
#PacketHeaderManager set tab_(PacketHeader/IP)         1
PacketHeaderManager set tab_(PacketHeader/HWMP)       1

Agent/HWMP  set debug_       0

Agent/HWMP  set sport_	     0
Agent/HWMP  set dport_	     0

Agent/HWMP instproc init args {
    $self next $args
}

Simulator instproc create-hwmp-agent { node } {
    set addr [$node node-addr]

    set ragent [new Agent/HWMP $addr]           ;# Create HWMP routing agent
    $ragent node $node
    if [Simulator set mobile_ip_] {
		$ragent port-dmux [$node demux]
	}

    $node addr $addr
    $node set ragent_ $ragent
    $self at 0.0 "$ragent start"                ;# Start BEACON/HELLO Message

    return $ragent
}

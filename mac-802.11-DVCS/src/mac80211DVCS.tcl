Mac/802_11/DVCS set CWMin_         31
Mac/802_11/DVCS set CWMax_         1023
Mac/802_11/DVCS set SlotTime_      0.000020        ;# 20us
Mac/802_11/DVCS set SIFS_          0.000010        ;# 10us
Mac/802_11/DVCS set PreambleLength_        144     ;# 144 bit
Mac/802_11/DVCS set PLCPHeaderLength_      48      ;# 48 bits
Mac/802_11/DVCS set PLCPDataRate_  1.0e6           ;# 1Mbps
Mac/802_11/DVCS set RTSThreshold_  0               ;# bytes
Mac/802_11/DVCS set ShortRetryLimit_ 7 ;# retransmittions
Mac/802_11/DVCS set LongRetryLimit_ 4 ;# retransmissions
Mac/802_11/DVCS set DirectionalRTSRetryLimit_ 4 ;# retransmissions

Mac/802_11/DVCS set bugFix_timer_ true;         # fix for when RTS/CTS not used
# details at http://www.dei.unipd.it/wdyn/?IDsezione=2435

Mac/802_11/DVCS set BeaconInterval_	       0.1		;# 100ms	
Mac/802_11/DVCS set ScanType_	PASSIVE
Mac/802_11/DVCS set ProbeDelay_	0.0001		        ;# 0.1 ms
Mac/802_11/DVCS set MaxChannelTime_ 0.011		    ;# 11 ms
Mac/802_11/DVCS set MinChannelTime_ 0.005		    ; # 5 ms
Mac/802_11/DVCS set ChannelTime_ 0.12		        ;# 120 ms
 
################################################################################
# Mobile Node
################################################################################

# Returns the antenna
Node/MobileNode instproc get-antenna { index } {
    $self instvar ant_
    return $ant_($index)
}

# Returns the mac with DVCS
Node/MobileNode instproc get-mac { index } {
    $self instvar mac_
    return $mac_($index)
}

# This is the modification of add-interface method in lib/ns-mobile.tcl to keep
# track of antenna(s) attached to a node through ant_ array
Node/MobileNode instproc add-interface { channel pmodel lltype mactype qtype qlen iftype anttype topo inerrproc outerrproc fecproc } {
	$self instvar arptable_ nifs_ netif_ mac_ ifq_ ll_ ant_ imep_ inerr_ outerr_ fec_

	set ns [Simulator instance]
	set imepflag [$ns imep-support]
	set t $nifs_
	incr nifs_

	set netif_($t)	[new $iftype]		;# interface
	set mac_($t)	[new $mactype]		;# mac layer
	set ifq_($t)	[new $qtype]		;# interface queue
	set ll_($t)	[new $lltype]		    ;# link layer
        set ant_($t)    [new $anttype]

	$ns mac-type $mactype
	set inerr_($t) ""
	if {$inerrproc != ""} {
		set inerr_($t) [$inerrproc]
	}
	set outerr_($t) ""
	if {$outerrproc != ""} {
		set outerr_($t) [$outerrproc]
	}
	set fec_($t) ""
	if {$fecproc != ""} {
		set fec_($t) [$fecproc]
	}

	set namfp [$ns get-nam-traceall]
        if {$imepflag == "ON" } {
		# IMEP layer
		set imep_($t) [new Agent/IMEP [$self id]]
		set imep $imep_($t)
		set drpT [$self mobility-trace Drop "RTR"]
		if { $namfp != "" } {
			$drpT namattach $namfp
		}
		$imep drop-target $drpT
		$ns at 0.[$self id] "$imep_($t) start"   ;# start beacon timer
        }
	#
	# Local Variables
	#
	set nullAgent_ [$ns set nullAgent_]
	set netif $netif_($t)
	set mac $mac_($t)
	set ifq $ifq_($t)
	set ll $ll_($t)

	set inerr $inerr_($t)
	set outerr $outerr_($t)
	set fec $fec_($t)

	#
	# Initialize ARP table only once.
	#
	if { $arptable_ == "" } {
		set arptable_ [new ARPTable $self $mac]
		# FOR backward compatibility sake, hack only
		if {$imepflag != ""} {
			set drpT [$self mobility-trace Drop "IFQ"]
		} else {
			set drpT [cmu-trace Drop "IFQ" $self]
		}
		$arptable_ drop-target $drpT
		if { $namfp != "" } {
			$drpT namattach $namfp
		}
        }
	#
	# Link Layer
	#
	$ll arptable $arptable_
	$ll mac $mac
	$ll down-target $ifq

	if {$imepflag == "ON" } {
		$imep recvtarget [$self entry]
		$imep sendtarget $ll
		$ll up-target $imep
        } else {
		$ll up-target [$self entry]
	}

	#
	# Interface Queue
	#
	$ifq target $mac
	$ifq set limit_ $qlen
	if {$imepflag != ""} {
		set drpT [$self mobility-trace Drop "IFQ"]
	} else {
		set drpT [cmu-trace Drop "IFQ" $self]
        }
	$ifq drop-target $drpT
	if { $namfp != "" } {
		$drpT namattach $namfp
	}
	if {[$ifq info class] == "Queue/XCP"} {
		$mac set bandwidth_ [$ll set bandwidth_]
		$mac set delay_ [$ll set delay_]
		$ifq set-link-capacity [$mac set bandwidth_]
		$ifq queue-limit $qlen
		$ifq link $ll
		$ifq reset

	}

	#
	# Mac Layer
	#

	$mac netif $netif
	$mac up-target $ll

	if {$outerr == "" && $fec == ""} {
		$mac down-target $netif
	} elseif {$outerr != "" && $fec == ""} {
		$mac down-target $outerr
		$outerr target $netif
	} elseif {$outerr == "" && $fec != ""} {
		$mac down-target $fec
		$fec down-target $netif
	} else {
		$mac down-target $fec
		$fec down-target $outerr
		$err target $netif
	}

	set god_ [God instance]
    if {$mactype == "Mac/802_11Ext"} {
		$mac nodes [$god_ num_nodes]
	}
    if {$mactype == "Mac/802_11"} {
		$mac nodes [$god_ num_nodes]
	}
	if {$mactype == "Mac/802_11/DVCS"} {
		$mac nodes [$god_ num_nodes]
	}
	#
	# Network Interface
	#
	#if {$fec == ""} {
        #		$netif up-target $mac
	#} else {
        #		$netif up-target $fec
	#	$fec up-target $mac
	#}

	$netif channel $channel
	if {$inerr == "" && $fec == ""} {
		$netif up-target $mac
	} elseif {$inerr != "" && $fec == ""} {
		$netif up-target $inerr
		$inerr target $mac
	} elseif {$err == "" && $fec != ""} {
		$netif up-target $fec
		$fec up-target $mac
	} else {
		$netif up-target $inerr
		$inerr target $fec
		$fec up-target $mac
	}

	$netif propagation $pmodel	;# Propagation Model
	$netif node $self		;# Bind node <---> interface
	$netif antenna $ant_($t)
	#
	# Physical Channel
	#
	$channel addif $netif

        # List-based improvement
	# For nodes talking to multiple channels this should
	# be called multiple times for each channel
	$channel add-node $self

	# let topo keep handle of channel
	$topo channel $channel
	# ============================================================

	if { [Simulator set PhyTrace_] == "ON" } {
		#
		# Trace Dropped Packets
		#
		if {$imepflag != ""} {
			set drpPhyT [$self mobility-trace Drop "PHY"]
		} else {
			set drpPhyT [cmu-trace Drop "PHY" $self]
		}
		$netif drop-target $drpPhyT
		if { $namfp != "" } {
			$drpPhyT namattach $namfp
		}

		#
		# Trace Sent Packets
		#
        if {$imepflag != ""} {
            set sndPhyT [$self mobility-trace Send "PHY"]
        } else {
            set sndPhyT [cmu-trace Send "PHY" $self]
        }
        $sndPhyT target $channel
        $netif down-target $sndPhyT
        if { $namfp != "" } {
            $sndPhyT namattach $namfp
        }

	} else {
		$netif drop-target [$ns set nullAgent_]
	}

	if { [Simulator set MacTrace_] == "ON" } {
		#
		# Trace RTS/CTS/ACK Packets
		#
		if {$imepflag != ""} {
			set rcvT [$self mobility-trace Recv "MAC"]
		} else {
			set rcvT [cmu-trace Recv "MAC" $self]
		}
		$mac log-target $rcvT
		if { $namfp != "" } {
			$rcvT namattach $namfp
		}
		#
		# Trace Sent Packets
		#
		if {$imepflag != ""} {
			set sndT [$self mobility-trace Send "MAC"]
		} else {
			set sndT [cmu-trace Send "MAC" $self]
		}
		$sndT target [$mac down-target]
		$mac down-target $sndT
		if { $namfp != "" } {
			$sndT namattach $namfp
		}
		#
		# Trace Received Packets
		#
		if {$imepflag != ""} {
			set rcvT [$self mobility-trace Recv "MAC"]
		} else {
			set rcvT [cmu-trace Recv "MAC" $self]
		}
		$rcvT target [$mac up-target]
		$mac up-target $rcvT
		if { $namfp != "" } {
			$rcvT namattach $namfp
		}
		#
		# Trace Dropped Packets
		#
		if {$imepflag != ""} {
			set drpT [$self mobility-trace Drop "MAC"]
		} else {
			set drpT [cmu-trace Drop "MAC" $self]
		}
		$mac drop-target $drpT
		if { $namfp != "" } {
			$drpT namattach $namfp
		}
	} else {
		$mac log-target [$ns set nullAgent_]
		$mac drop-target [$ns set nullAgent_]
	}

# change wrt Mike's code
       if { [Simulator set EotTrace_] == "ON" } {
               #
               # Also trace end of transmission time for packets
               #
               if {$imepflag != ""} {
                       set eotT [$self mobility-trace EOT "MAC"]
               } else {
                       set eoT [cmu-trace EOT "MAC" $self]
               }
               $mac eot-target $eotT
       }

	# ============================================================
	$self addif $netif
}

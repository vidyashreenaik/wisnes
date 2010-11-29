lib_LTLIBRARIES = libaodvrtt.la

libaodvrtt_la_SOURCES = aodvrtt.cc\
                    aodvrtt.h\
                    aodvrtt_logs.cc\
                    aodvrtt_packet_classifier.cc\
                    aodvrtt_packet_classifier.h\
                    aodvrtt_packet_tracer.cc\
                    aodvrtt_packet_tracer.h\
                    aodvrtt_packet.h\
                    aodvrtt_rqueue.cc\
                    aodvrtt_rqueue.h\
                    aodvrtt_rtable.cc\
                    aodvrtt_rtable.h\
                    init.cc

nodist_libaodvrtt_la_SOURCES = embeddedtcl.cc

libaodvrtt_la_CPPFLAGS = @NS_CPPFLAGS@
libaodvrtt_la_LDFLAGS = @NS_LDFLAGS@
libaodvrtt_la_LIBADD = @NS_LIBADD@

BUILT_SOURCES = embeddedtcl.cc

CLEANFILES = embeddedtcl.cc

TCL_FILES = aodvrtt.tcl

embeddedtcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ AodvrttTclCode > embeddedtcl.cc

EXTRA_DIST =  $(TCL_FILES)


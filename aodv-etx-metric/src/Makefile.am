lib_LTLIBRARIES = libaodvetx.la

libaodvetx_la_SOURCES = aodvetx.cc\
                    aodvetx.h\
                    aodvetx_logs.cc\
                    aodvetx_packet_classifier.cc\
                    aodvetx_packet_classifier.h\
                    aodvetx_packet_tracer.cc\
                    aodvetx_packet_tracer.h\
                    aodvetx_packet.h\
                    aodvetx_rqueue.cc\
                    aodvetx_rqueue.h\
                    aodvetx_rtable.cc\
                    aodvetx_rtable.h\
                    init.cc

nodist_libaodvetx_la_SOURCES = embeddedtcl.cc

libaodvetx_la_CPPFLAGS = @NS_CPPFLAGS@
libaodvetx_la_LDFLAGS = @NS_LDFLAGS@
libaodvetx_la_LIBADD = @NS_LIBADD@

BUILT_SOURCES = embeddedtcl.cc

CLEANFILES = embeddedtcl.cc

TCL_FILES = aodvetx.tcl

embeddedtcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ AodvetxTclCode > embeddedtcl.cc

EXTRA_DIST =  $(TCL_FILES)


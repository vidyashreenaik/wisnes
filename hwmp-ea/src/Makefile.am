lib_LTLIBRARIES = libhwmp.la

libhwmp_la_SOURCES = hwmp.cc\
                    hwmp.h\
                    hwmp_packet_classifier.cc\
                    hwmp_packet_classifier.h\
                    hwmp_packet_tracer.cc\
                    hwmp_packet_tracer.h\
                    hwmp_packet.h\
                    hwmp_rqueue.cc\
                    hwmp_rqueue.h\
                    hwmp_rtable.cc\
                    hwmp_rtable.h\
                    init.cc

nodist_libhwmp_la_SOURCES = embeddedtcl.cc

libhwmp_la_CPPFLAGS = @NS_CPPFLAGS@
libhwmp_la_LDFLAGS = @NS_LDFLAGS@
libhwmp_la_LIBADD = @NS_LIBADD@

BUILT_SOURCES = embeddedtcl.cc

CLEANFILES = embeddedtcl.cc

TCL_FILES = hwmp.tcl

embeddedtcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ HwmpTclCode > embeddedtcl.cc

EXTRA_DIST =  $(TCL_FILES)


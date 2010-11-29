lib_LTLIBRARIES = libnsmiracleaodv.la

libnsmiracleaodv_la_SOURCES =  nsmiracle_aodv.cc\
                    nsmiracle_aodv.h\
                    nsmiracle_aodv_logs.cc\
                    nsmiracle_aodv_packet_classifier.cc\
                    nsmiracle_aodv_packet_classifier.h\
                    nsmiracle_aodv_packet_tracer.cc\
                    nsmiracle_aodv_packet_tracer.h\
                    nsmiracle_aodv_packet.h\
                    nsmiracle_aodv_rqueue.cc\
                    nsmiracle_aodv_rqueue.h\
                    nsmiracle_aodv_rtable.cc\
                    nsmiracle_aodv_rtable.h\
                    nsmiracle_aodv_interface.cc\
                    nsmiracle_aodv_interface.h\
                    nsmiracle_aodv_module.cc\
                    nsmiracle_aodv_module.h\
                    nsmiracle_aodv_init.cc

nodist_libnsmiracleaodv_la_SOURCES = InitTcl.cc

libnsmiracleaodv_la_CPPFLAGS = @NS_CPPFLAGS@ @NSMIRACLE_CPPFLAGS@
libnsmiracleaodv_la_LDFLAGS = @NS_LDFLAGS@ @NSMIRACLE_LDFLAGS@
libnsmiracleaodv_la_LIBADD = @NS_LIBADD@ @NSMIRACLE_LIBADD@ -lMiracle -lMiracleIp

BUILT_SOURCES = InitTcl.cc

CLEANFILES = InitTcl.cc

TCL_FILES = nsmiracle_aodv.tcl

InitTcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ NsmiracleaodvTclCode > InitTcl.cc

EXTRA_DIST =  $(TCL_FILES)


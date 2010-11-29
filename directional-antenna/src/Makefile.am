lib_LTLIBRARIES = libdirectional_antenna.la

libdirectional_antenna_la_SOURCES = directional_antenna.cc\
                directional_antenna.h\
                flattop_directional_antenna.cc\
                flattop_directional_antenna.h\
                pattern_directional_antenna.cc\
                pattern_directional_antenna.h\
                smart_directional_antenna.cc\
                smart_directional_antenna.h\
                initlib.cc

nodist_libdirectional_antenna_la_SOURCES = embeddedtcl.cc

libdirectional_antenna_la_CPPFLAGS = @NS_CPPFLAGS@
libdirectional_antenna_la_LDFLAGS = @NS_LDFLAGS@
libdirectional_antenna_la_LIBADD = @NS_LIBADD@

BUILT_SOURCES = embeddedtcl.cc

CLEANFILES = embeddedtcl.cc

TCL_FILES = directional_antenna.tcl

embeddedtcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Directional_antennaTclCode > embeddedtcl.cc

EXTRA_DIST =  $(TCL_FILES) antenna.pattern


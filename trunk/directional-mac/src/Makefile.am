lib_LTLIBRARIES = libdirectional_mac.la

libdirectional_mac_la_SOURCES = directional_mac.cc\
                directional_mac.h\
                wireless_phy_interface.cc\
                wireless_phy_interface.h\
                initlib.cc

nodist_libdirectional_mac_la_SOURCES = embeddedtcl.cc

libdirectional_mac_la_CPPFLAGS = @NS_CPPFLAGS@
libdirectional_mac_la_LDFLAGS = @NS_LDFLAGS@
libdirectional_mac_la_LIBADD = @NS_LIBADD@

BUILT_SOURCES = embeddedtcl.cc

CLEANFILES = embeddedtcl.cc

TCL_FILES = directional_mac.tcl

embeddedtcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Directional_macTclCode > embeddedtcl.cc

EXTRA_DIST =  $(TCL_FILES)

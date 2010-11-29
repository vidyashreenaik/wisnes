lib_LTLIBRARIES = libmac_dvcs.la

libmac_dvcs_la_SOURCES = wireless-phy-DVCS.cc\
                wireless-phy-DVCS.h\
                mac-802_11-DVCS.cc\
                mac-802_11-DVCS.h\
                initlib.cc

nodist_libmac_dvcs_la_SOURCES = embeddedtcl.cc

libmac_dvcs_la_CPPFLAGS = @NS_CPPFLAGS@ -I../../directional-antenna/src/
libmac_dvcs_la_LDFLAGS = @NS_LDFLAGS@
libmac_dvcs_la_LIBADD = @NS_LIBADD@ -ldirectional_antenna

BUILT_SOURCES = embeddedtcl.cc

CLEANFILES = embeddedtcl.cc

TCL_FILES = mac80211DVCS.tcl

embeddedtcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Mac_dvcsTclCode > embeddedtcl.cc

EXTRA_DIST =  $(TCL_FILES)


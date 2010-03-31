lib_LTLIBRARIES = libmac80211DVCS.la

libmac80211DVCS_la_SOURCES = mac-802_11-DVCS.cc\
                mac-802_11-DVCS.h\
                mac-802_11-DVCS-timers.cc\
                mac-802_11-DVCS-timers.h\
                initlib.cc

nodist_libmac80211DVCS_la_SOURCES = embeddedtcl.cc

libmac80211DVCS_la_CPPFLAGS = @NS_CPPFLAGS@
libmac80211DVCS_la_LDFLAGS = @NS_LDFLAGS@
libmac80211DVCS_la_LIBADD = @NS_LIBADD@

BUILT_SOURCES = embeddedtcl.cc

CLEANFILES = embeddedtcl.cc

TCL_FILES = mac80211DVCS.tcl

embeddedtcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Mac80211DVCSTclCode > embeddedtcl.cc

EXTRA_DIST =  $(TCL_FILES)


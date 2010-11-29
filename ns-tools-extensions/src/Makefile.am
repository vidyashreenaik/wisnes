lib_LTLIBRARIES = libns_tools_extensions.la

libns_tools_extensions_la_SOURCES = poisson_random_variable.cc \
                poisson_random_variable.h \
                ns_tools_math.h \
                ns_tools_math.cc \
                initlib.cc

nodist_libns_tools_extensions_la_SOURCES = embeddedtcl.cc

libns_tools_extensions_la_CPPFLAGS = @NS_CPPFLAGS@
libns_tools_extensions_la_LDFLAGS = @NS_LDFLAGS@
libns_tools_extensions_la_LIBADD = @NS_LIBADD@

BUILT_SOURCES = embeddedtcl.cc

CLEANFILES = embeddedtcl.cc

TCL_FILES = ns_tools_extensions.tcl

embeddedtcl.cc: Makefile $(TCL_FILES)
		cat $(TCL_FILES) | @TCL2CPP@ Ns_tools_extensionsTclCode > embeddedtcl.cc

EXTRA_DIST =  $(TCL_FILES)


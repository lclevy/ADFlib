
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src doc
if EXAMPLES
SUBDIRS += examples
endif
if REGTESTS
SUBDIRS += regtests/Test
endif

SUBDIRS += tests

EXTRA_DIST = regtests/Boot regtests/Dumps regtests/Files

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = adflib.pc


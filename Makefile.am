
ACLOCAL_AMFLAGS = -I m4

SUBDIRS = src doc
if EXAMPLES
SUBDIRS += examples
endif
if REGTESTS
SUBDIRS += regtests/Test
endif

SUBDIRS += tests

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = adflib.pc


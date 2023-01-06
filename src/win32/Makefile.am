
AM_CPPFLAGS = -D_XOPEN_SOURCE -D_SVID_SOURCE -D_BSD_SOURCE -D_DEFAULT_SOURCE \
    -D_GNU_SOURCE -I$(topsrcdir)/src
AM_CFLAGS = -std=c99 -pedantic -Wall -Wextra -fPIC -I$(top_srcdir)/src


lib_LIBRARIES = libadfnative.a

libadfnative_a_SOURCES = adf_nativ.c \
	nt4_dev.c

include_HEADERS = adf_nativ.h \
	defendian.h \
	nt4_dev.h

libadfnative_a_LIBADD = adf_nativ.o \
	nt4_dev.o

#lib_LTLIBRARIES = libadfnative.la
#libadfnative_la_SOURCES = adf_nativ.c
#include_HEADERS = adf_nativ.h
##libadfnative_la_LDFLAGS = adf_nativ.o



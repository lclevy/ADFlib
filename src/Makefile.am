
AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -D_XOPEN_SOURCE -D_SVID_SOURCE -D_BSD_SOURCE -D_DEFAULT_SOURCE \
    -D_GNU_SOURCE -D_EXPORTING
#	-DADFLIB_VERSION='"$(ADFLIB_VERSION)"' -DADFLIB_DATE='"$(ADFLIB_DATE)"'
AM_CFLAGS = -std=c99 -pedantic -Wall -Wextra \
    -Werror-implicit-function-declaration \
    -Werror=format-security


lib_LTLIBRARIES = libadf.la

libadf_la_SOURCES = \
    adf_bitm.c \
    adf_cache.c \
    adf_dev.c \
    adf_dev_dump.c \
    adf_dev_flop.c \
    adf_dev_hd.c \
    adf_dir.c \
    adf_env.c \
    adf_file_block.c \
    adf_file.c \
    adf_link.c \
    adf_raw.c \
    adf_salv.c \
    adf_str.c \
    adf_util.c \
    adf_vol.c
#    debug_util.c

adfincdir = $(includedir)/adf
adfinc_HEADERS = \
    adflib.h \
    adf_bitm.h \
    adf_blk.h \
    adf_cache.h \
    adf_dev_dump.h \
    adf_dev_flop.h \
    adf_dev.h \
    adf_dev_hd.h \
    adf_dir.h \
    adf_env.h \
    adf_err.h \
    adf_file_block.h \
    adf_file.h \
    adf_file_util.h \
    adf_link.h \
    adf_nativ.h \
    adf_raw.h \
    adf_salv.h \
    adf_str.h \
    adf_types.h \
    adf_util.h \
    adf_version.h \
    adf_vol.h \
    debug_util.h \
    defendian.h \
    hd_blk.h \
    prefix.h

libadf_la_LDFLAGS = -version-info $(ADFLIB_LT_VERSION)

if NATIVE_GENERIC
libadf_la_SOURCES += generic/adf_nativ.c
endif
if NATIVE_LINUX
libadf_la_SOURCES += linux/adf_nativ.c
endif
if NATIVE_WIN32
libadf_la_SOURCES += win32/adf_nativ.c win32/nt4_dev.c win32/nt4_dev.h
AM_CFLAGS += -I$(srcdir)/win32
endif

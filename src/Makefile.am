
AUTOMAKE_OPTIONS = subdir-objects

AM_CFLAGS = -std=c99 -pedantic -Wall -Wextra \
    -Werror-implicit-function-declaration \
    -Werror=format-security

AM_CPPFLAGS = -D_XOPEN_SOURCE -D_SVID_SOURCE -D_BSD_SOURCE -D_DEFAULT_SOURCE \
    -D_GNU_SOURCE -D_EXPORTING
#	-DADFLIB_VERSION='"$(ADFLIB_VERSION)"' -DADFLIB_DATE='"$(ADFLIB_DATE)"'


lib_LTLIBRARIES = libadf.la

libadf_la_SOURCES = \
    adf_bitm.c \
    adf_byteorder.h \
    adf_cache.c \
    adf_dev.c \
    adf_dev_driver_dump.c \
    adf_dev_driver_ramdisk.c \
    adf_dev_drivers.c \
    adf_dev_flop.c \
    adf_dev_hd.c \
    adf_dir.c \
    adf_env.c \
    adf_file_block.c \
    adf_file.c \
    adf_link.c \
    adf_link.h \
    adf_raw.c \
    adf_salv.c \
    adf_str.c \
    adf_util.c \
    adf_util.h \
    adf_vol.c
#    debug_util.h \
#    debug_util.c

# ADFlib's private headers should be listed only in _SOURCES, not in _HEADERS.
# More info:
# - https://stackoverflow.com/questions/78486890/how-to-properly-specify-public-and-private-headers-with-autotools-library-in-c
# - https://www.gnu.org/software/automake/manual/automake.html#Headers
adfincdir = $(includedir)/adf
adfinc_HEADERS = \
    adflib.h \
    adf_bitm.h \
    adf_blk.h \
    adf_blk_hd.h \
    adf_cache.h \
    adf_dev_driver.h \
    adf_dev_driver_dump.h \
    adf_dev_driver_nativ.h \
    adf_dev_driver_ramdisk.h \
    adf_dev_drivers.h \
    adf_dev_flop.h \
    adf_dev.h \
    adf_dev_hd.h \
    adf_dir.h \
    adf_env.h \
    adf_err.h \
    adf_file_block.h \
    adf_file.h \
    adf_file_util.h \
    adf_prefix.h \
    adf_raw.h \
    adf_salv.h \
    adf_str.h \
    adf_types.h \
    adf_version.h \
    adf_vol.h

libadf_la_LDFLAGS = -version-info $(ADFLIB_LT_VERSION)

if NATIVE_GENERIC
libadf_la_SOURCES += generic/adf_dev_driver_nativ.c
endif
if NATIVE_LINUX
libadf_la_SOURCES += linux/adf_dev_driver_nativ.c
endif
if NATIVE_WIN32
libadf_la_SOURCES += win32/adf_dev_driver_nativ.c win32/nt4_dev.c win32/nt4_dev.h
AM_CFLAGS += -I$(srcdir)/win32
endif

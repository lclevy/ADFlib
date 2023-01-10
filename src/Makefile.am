
SUBDIRS = $(NATIVE_DIR)

lib_LTLIBRARIES = libadf.la

libadf_la_SOURCES = adf_hd.c \
                    adf_dev.c \
                    adf_dev_flop.c \
                    adf_disk.c \
                    adf_raw.c \
                    adf_bitm.c \
                    adf_dump.c \
                    adf_util.c \
                    adf_env.c \
                    adf_dir.c \
                    adf_file.c \
                    adf_cache.c \
                    adf_link.c \
                    adf_salv.c

include_HEADERS = adf_defs.h \
                  adf_dev.h \
                  adf_dev_flop.h \
                  adf_blk.h \
                  adf_err.h \
                  adf_str.h \
                  adflib.h \
                  adf_bitm.h \
                  adf_cache.h \
                  adf_dir.h \
                  adf_disk.h \
                  adf_dump.h \
                  adf_env.h \
                  adf_file.h \
                  adf_hd.h \
                  adf_link.h \
                  adf_raw.h \
                  adf_salv.h \
                  adf_util.h \
                  defendian.h \
                  hd_blk.h \
                  prefix.h

libadf_la_LDFLAGS = -version-info 0:12:0

# Link adfnative
if HOSTOSCYGWIN
    # CygWin autotools links adfnative by itself
else
    # Link adfnative to libadf
    libadf_la_LDFLAGS += -L$(NATIVE_DIR) -ladfnative
endif

AM_CPPFLAGS = -D_XOPEN_SOURCE -D_SVID_SOURCE -D_BSD_SOURCE -D_DEFAULT_SOURCE \
    -D_GNU_SOURCE

AM_CFLAGS = -std=c99 -pedantic -Wall -Wextra \
    -I$(srcdir) \
    -I$(srcdir)/$(NATIVE_DIR)

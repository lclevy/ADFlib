
AUTOMAKE_OPTIONS = subdir-objects

AM_CPPFLAGS = -std=c99 -Wall -Wextra -pedantic \
    -Werror-implicit-function-declaration \
    -I$(top_srcdir)/src -I$(top_srcdir)/src/$(NATIVE_DIR)

check_PROGRAMS = \
    test_adfDays2Date \
    test_adfPos2DataBlock \
    test_dev_open \
    test_dev_mount\
    test_adf_file_util \
    test_file_append \
    test_file_create \
    test_file_overwrite \
    test_file_overwrite2 \
    test_file_seek \
    test_file_seek_after_write \
    test_file_truncate \
    test_file_truncate2 \
    test_file_write \
    test_file_write_chunks \
    test_test_util

TESTS = $(check_PROGRAMS)

ADFLIBS = $(top_builddir)/src/libadf.la

test_adfDays2Date_SOURCES = test_adfDays2Date.c
test_adfDays2Date_CFLAGS = $(CHECK_CFLAGS)
test_adfDays2Date_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_adfDays2Date_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_adfPos2DataBlock_SOURCES = test_adfPos2DataBlock.c \
    $(top_builddir)/src/adf_util.c
test_adfPos2DataBlock_CFLAGS = $(CHECK_CFLAGS)
test_adfPos2DataBlock_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_adfPos2DataBlock_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_dev_open_SOURCES = test_dev_open.c
test_dev_open_CFLAGS = $(CHECK_CFLAGS)
test_dev_open_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_dev_open_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_dev_mount_SOURCES = test_dev_mount.c
test_dev_mount_CFLAGS = $(CHECK_CFLAGS)
test_dev_mount_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_dev_mount_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_adf_file_util_SOURCES = test_adf_file_util.c
test_adf_file_util_CFLAGS = $(CHECK_CFLAGS)
test_adf_file_util_LDADD = $(CHECK_LIBS)
#test_adf_file_util_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_file_create_SOURCES = test_file_create.c
test_file_create_CFLAGS = $(CHECK_CFLAGS)
test_file_create_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_file_create_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_file_append_SOURCES = test_file_append.c
test_file_append_CFLAGS = $(CHECK_CFLAGS)
test_file_append_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_file_append_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_file_write_SOURCES = test_file_write.c test_util.c test_util.h
test_file_write_CFLAGS = $(CHECK_CFLAGS)
test_file_write_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_file_write_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_file_write_chunks_SOURCES = test_file_write_chunks.c test_util.c test_util.h
test_file_write_chunks_CFLAGS = $(CHECK_CFLAGS)
test_file_write_chunks_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_file_write_chunks_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_file_overwrite_SOURCES = test_file_overwrite.c
test_file_overwrite_CFLAGS = $(CHECK_CFLAGS)
test_file_overwrite_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_file_overwrite_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_file_overwrite2_SOURCES = test_file_overwrite2.c test_util.c test_util.h
test_file_overwrite2_CFLAGS = $(CHECK_CFLAGS)
test_file_overwrite2_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_file_overwrite2_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_file_seek_SOURCES = test_file_seek.c test_util.c test_util.h
test_file_seek_CFLAGS = $(CHECK_CFLAGS)
test_file_seek_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_file_seek_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_file_seek_after_write_SOURCES = test_file_seek_after_write.c test_util.c test_util.h
test_file_seek_after_write_CFLAGS = $(CHECK_CFLAGS)
test_file_seek_after_write_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_file_seek_after_write_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_file_truncate_SOURCES = test_file_truncate.c test_util.c test_util.h
test_file_truncate_CFLAGS = $(CHECK_CFLAGS)
test_file_truncate_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_file_truncate_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_file_truncate2_SOURCES = test_file_truncate2.c test_util.c test_util.h
test_file_truncate2_CFLAGS = $(CHECK_CFLAGS)
test_file_truncate2_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_file_truncate2_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_test_util_SOURCES = test_test_util.c test_util.c test_util.h
test_test_util_CFLAGS = $(CHECK_CFLAGS)
test_test_util_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_test_util_DEPENDENCIES = $(top_builddir)/src/libadf.la

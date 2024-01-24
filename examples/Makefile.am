bin_PROGRAMS = unadf \
	adf_bitmap \
	adf_floppy_create \
	adf_floppy_format \
	adf_show_metadata

dist_check_SCRIPTS = \
	tests/test_examples_basic.sh \
	tests/adf-floppy-test.sh \
	tests/adf-show-metadata-test.sh \
	tests/unadf-test.sh
dist_check_DATA = \
	tests/arccsh.adf \
	tests/common.sh \
	tests/win32-names.adf

TESTS = $(dist_check_SCRIPTS)

ADFLIBS = $(top_builddir)/src/libadf.la

unadf_SOURCES = unadf.c
unadf_LDADD = $(ADFLIBS)
unadf_DEPENDENCIES = $(top_builddir)/src/libadf.la

adf_bitmap_SOURCES = adf_bitmap.c
adf_bitmap_LDADD = $(ADFLIBS)
adf_bitmap_DEPENDENCIES = $(top_builddir)/src/libadf.la

adf_floppy_create_SOURCES = adf_floppy_create.c
adf_floppy_create_LDADD = $(ADFLIBS)
adf_floppy_create_DEPENDENCIES = $(top_builddir)/src/libadf.la

adf_floppy_format_SOURCES = adf_floppy_format.c
adf_floppy_format_LDADD = $(ADFLIBS)
adf_floppy_format_DEPENDENCIES = $(top_builddir)/src/libadf.la

adf_show_metadata_SOURCES = adf_show_metadata.c \
  adf_show_metadata_common.c \
  adf_show_metadata_common.h \
  adf_show_metadata_dir.c \
  adf_show_metadata_dir.h \
  adf_show_metadata_file.c \
  adf_show_metadata_file.h \
  adf_show_metadata_volume.c \
  adf_show_metadata_volume.h
adf_show_metadata_LDADD = $(ADFLIBS)
adf_show_metadata_DEPENDENCIES = $(top_builddir)/src/libadf.la

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/$(NATIVE_DIR)


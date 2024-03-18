bin_PROGRAMS = unadf \
	adf_bitmap \
	adf_floppy_create \
	adf_format \
	adf_show_metadata

dist_check_SCRIPTS = \
	tests/test_examples_basic.sh \
	tests/adf-floppy-test.sh \
	tests/adf-show-metadata-test.sh \
	tests/unadf-test.sh
dist_check_DATA = \
	tests/arccsh.adf \
	tests/common.sh \
	tests/win32-names.adf \
	tests/results/adf-floppy-test_1 \
	tests/results/adf-floppy-test_2 \
	tests/results/adf-floppy-test_3 \
	tests/results/adf-floppy-test_4 \
	tests/results/adf-show-metadata-test_1 \
	tests/results/adf-show-metadata-test_2 \
	tests/results/adf-show-metadata-test_3 \
	tests/results/adf-show-metadata-test_4 \
	tests/results/adf-show-metadata-test_5 \
	tests/results/unadf_1 \
	tests/results/unadf_2 \
	tests/results/unadf_3 \
	tests/results/unadf_4 \
	tests/results/unadf_5 \
	tests/results/unadf_6 \
	tests/results/unadf_7 \
	tests/results/unadf_8 \
	tests/results/unadf_9 \
	tests/results/unadf_10 \
	tests/results/unadf_11 \
	tests/results_msys/adf-floppy-test_1 \
	tests/results_msys/adf-floppy-test_2 \
	tests/results_msys/adf-floppy-test_3 \
	tests/results_msys/adf-floppy-test_4 \
	tests/results_msys/adf-show-metadata-test_1 \
	tests/results_msys/adf-show-metadata-test_2 \
	tests/results_msys/adf-show-metadata-test_3 \
	tests/results_msys/adf-show-metadata-test_4 \
	tests/results_msys/adf-show-metadata-test_5 \
	tests/results_msys/unadf_1 \
	tests/results_msys/unadf_2 \
	tests/results_msys/unadf_3 \
	tests/results_msys/unadf_4 \
	tests/results_msys/unadf_5 \
	tests/results_msys/unadf_6 \
	tests/results_msys/unadf_7 \
	tests/results_msys/unadf_8 \
	tests/results_msys/unadf_9 \
	tests/results_msys/unadf_10 \
	tests/results_msys/unadf_11

check_SCRIPTS = $(dist_check_SCRIPTS)
check_DATA = $(dist_check_DATA)

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

adf_format_SOURCES = adf_format.c
adf_format_LDADD = $(ADFLIBS)
adf_format_DEPENDENCIES = $(top_builddir)/src/libadf.la

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


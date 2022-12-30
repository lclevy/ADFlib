bin_PROGRAMS = unadf adf_floppy_create adf_floppy_format

unadf_SOURCES = unadf.c
unadf_LDADD = $(top_builddir)/src/libadf.la
unadf_DEPENDENCIES = $(top_builddir)/src/libadf.la

adf_floppy_create_SOURCES = adf_floppy_create.c
adf_floppy_create_LDADD = $(top_builddir)/src/libadf.la
adf_floppy_create_DEPENDENCIES = $(top_builddir)/src/libadf.la

adf_floppy_format_SOURCES = adf_floppy_format.c
adf_floppy_format_LDADD = $(top_builddir)/src/libadf.la
adf_floppy_format_DEPENDENCIES = $(top_builddir)/src/libadf.la

AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/$(NATIVE_DIR)


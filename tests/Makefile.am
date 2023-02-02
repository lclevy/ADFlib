
AM_CPPFLAGS = -I$(top_srcdir)/src -I$(top_srcdir)/src/$(NATIVE_DIR)

check_PROGRAMS = \
    test_adfDays2Date \
    test_adfPos2DataBlock

TESTS = $(check_PROGRAMS) \
    test_examples

ADFLIBS = $(top_builddir)/src/libadf.la
if ADFLIB_OS_CYGWIN
    ADFLIBS += $(top_builddir)/src/$(NATIVE_DIR)/libadfnative.a
endif

test_adfDays2Date_SOURCES = test_adfDays2Date.c
test_adfDays2Date_CFLAGS = $(CHECK_CFLAGS)
test_adfDays2Date_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_adfDays2Date_DEPENDENCIES = $(top_builddir)/src/libadf.la

test_adfPos2DataBlock_SOURCES = test_adfPos2DataBlock.c
test_adfPos2DataBlock_CFLAGS = $(CHECK_CFLAGS)
test_adfPos2DataBlock_LDADD = $(ADFLIBS) $(CHECK_LIBS)
test_adfPos2DataBlock_DEPENDENCIES = $(top_builddir)/src/libadf.la

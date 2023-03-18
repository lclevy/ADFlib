

AM_CPPFLAGS = -D_XOPEN_SOURCE -D_SVID_SOURCE -D_BSD_SOURCE -D_DEFAULT_SOURCE \
    -D_GNU_SOURCE
AM_CFLAGS = -std=c99 -pedantic -Wall -Wextra \
    -Werror-implicit-function-declaration \
    -I$(top_srcdir)/src

check_PROGRAMS = \
	access \
	bootdisk \
	comment \
	del_test \
	dir_test \
	dir_test2 \
	dir_test_chdir \
	dispsect \
	file_test \
	file_test2 \
	file_test3 \
	file_read_hard_link_test \
	file_seek_test \
	file_seek_test2 \
	fl_test \
	fl_test2 \
	floppy_overfilling_test \
	hardfile \
	hardfile2 \
	hd_test \
	hd_test2 \
	hd_test3 \
	progbar \
	readonly \
	rename \
	rename2 \
	undel \
	undel2 \
	undel3

#TESTS = $(check_PROGRAMS)
TESTS = floppy.sh bigdev.sh
#EXTRA_DIST = $(TESTS)

ADFLIBS = $(top_builddir)/src/libadf.la
if ADFLIB_OS_CYGWIN
    ADFLIBS += $(top_builddir)/src/$(NATIVE_DIR)/libadfnative.a
endif

fl_test_SOURCES = fl_test.c
fl_test_LDADD = $(ADFLIBS)
fl_test_DEPENDENCIES = $(top_builddir)/src/libadf.la

fl_test2_SOURCES = fl_test2.c
fl_test2_LDADD = $(ADFLIBS)
fl_test2_DEPENDENCIES = $(top_builddir)/src/libadf.la

dir_test_SOURCES = dir_test.c
dir_test_LDADD = $(ADFLIBS)
dir_test_DEPENDENCIES = $(top_builddir)/src/libadf.la

dir_test2_SOURCES = dir_test2.c
dir_test2_LDADD = $(ADFLIBS)
dir_test2_DEPENDENCIES = $(top_builddir)/src/libadf.la

dir_test_chdir_SOURCES = dir_test_chdir.c
dir_test_chdir_LDADD = $(ADFLIBS)
dir_test_chdir_DEPENDENCIES = $(top_builddir)/src/libadf.la

hd_test_SOURCES = hd_test.c
hd_test_LDADD = $(ADFLIBS)
hd_test_DEPENDENCIES = $(top_builddir)/src/libadf.la

hd_test2_SOURCES = hd_test2.c
hd_test2_LDADD = $(ADFLIBS)
hd_test2_DEPENDENCIES = $(top_builddir)/src/libadf.la

hd_test3_SOURCES = hd_test3.c
hd_test3_LDADD = $(ADFLIBS)
hd_test3_DEPENDENCIES = $(top_builddir)/src/libadf.la

file_test_SOURCES = file_test.c
file_test_LDADD = $(ADFLIBS)
file_test_DEPENDENCIES = $(top_builddir)/src/libadf.la

file_test2_SOURCES = file_test2.c
file_test2_LDADD = $(ADFLIBS)
file_test2_DEPENDENCIES = $(top_builddir)/src/libadf.la

file_test3_SOURCES = file_test3.c
file_test3_LDADD = $(ADFLIBS)
file_test3_DEPENDENCIES = $(top_builddir)/src/libadf.la

file_seek_test_SOURCES = file_seek_test.c
file_seek_test_LDADD = $(ADFLIBS)
file_seek_test_DEPENDENCIES = $(top_builddir)/src/libadf.la

file_seek_test2_SOURCES = file_seek_test2.c
file_seek_test2_LDADD = $(ADFLIBS)
file_seek_test2_DEPENDENCIES = $(top_builddir)/src/libadf.la

file_read_hard_link_test_SOURCES = file_read_hard_link_test.c
file_read_hard_link_test_LDADD = $(ADFLIBS)
file_read_hard_link_test_DEPENDENCIES = $(top_builddir)/src/libadf.la

del_test_SOURCES = del_test.c
del_test_LDADD = $(ADFLIBS)
del_test_DEPENDENCIES = $(top_builddir)/src/libadf.la

bootdisk_SOURCES = bootdisk.c
bootdisk_LDADD = $(ADFLIBS)
bootdisk_DEPENDENCIES = $(top_builddir)/src/libadf.la

rename_SOURCES = rename.c
rename_LDADD = $(ADFLIBS)
rename_DEPENDENCIES = $(top_builddir)/src/libadf.la

hardfile_SOURCES = hardfile.c
hardfile_LDADD = $(ADFLIBS)
hardfile_DEPENDENCIES = $(top_builddir)/src/libadf.la

rename2_SOURCES = rename2.c
rename2_LDADD = $(ADFLIBS)
rename2_DEPENDENCIES = $(top_builddir)/src/libadf.la

hardfile2_SOURCES = hardfile2.c
hardfile2_LDADD = $(ADFLIBS)
hardfile2_DEPENDENCIES = $(top_builddir)/src/libadf.la

access_SOURCES = access.c
access_LDADD = $(ADFLIBS)
access_DEPENDENCIES = $(top_builddir)/src/libadf.la

comment_SOURCES = comment.c
comment_LDADD = $(ADFLIBS)
comment_DEPENDENCIES = $(top_builddir)/src/libadf.la

undel_SOURCES = undel.c
undel_LDADD = $(ADFLIBS)
undel_DEPENDENCIES = $(top_builddir)/src/libadf.la

readonly_SOURCES = readonly.c
readonly_LDADD = $(ADFLIBS)
readonly_DEPENDENCIES = $(top_builddir)/src/libadf.la

undel2_SOURCES = undel2.c
undel2_LDADD = $(ADFLIBS)
undel2_DEPENDENCIES = $(top_builddir)/src/libadf.la

dispsect_SOURCES = dispsect.c
dispsect_LDADD = $(ADFLIBS)
dispsect_DEPENDENCIES = $(top_builddir)/src/libadf.la

progbar_SOURCES = progbar.c
progbar_LDADD = $(ADFLIBS)
progbar_DEPENDENCIES = $(top_builddir)/src/libadf.la

undel3_SOURCES = undel3.c
undel3_LDADD = $(ADFLIBS)
undel3_DEPENDENCIES = $(top_builddir)/src/libadf.la

floppy_overfilling_test_SOURCES = floppy_overfilling_test.c
floppy_overfilling_test_LDADD = $(ADFLIBS)
floppy_overfilling_test_DEPENDENCIES = $(top_builddir)/src/libadf.la

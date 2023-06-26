# ADFlib (Amiga Disk File library)

## Introduction

The `ADFlib` is a free, portable and open implementation of the Amiga filesystem.

The initial release was in 1999.

It supports:
- floppy and hard disk images ("dumps")
- mount, unmount, create a device image (an adf file) or a volume (a partition
  inside a device)
- create, open, close, delete, rename/move a file or a directory
- file operations: read, write, truncate
- directory operations: get contents, change current, get parent
- use dir cache to get directory contents
- hard- and softlinks for accessing files and directories

Untested and/or exprerimental support exists for:
- WinNT and Linux physical devices (with the 'native driver')
- multiple partitions harddisk dumps
- UAE hardfiles
- undel files and directories

It is written in portable C.

The current version was built and tested in the CI system (GitHub Actions)
on Linux (Debian 11 and 12, Ubuntu), Windows (native build with MSVC and
CygWin) and MacOs.

It should be possible to build on (or cross-compile for) other systems.


## Command-line utilities

The `examples/` directory contains few useful command-line utilities
(as well as examples of the library usage).

Usage info is shown when they are executed without any parameters
(see also man pages).


### unADF

`unADF` is a unzip like utility for .ADF files:

```
unadf [-lrcsp -v n] dumpname.adf [files-with-path] [-d extractdir]
    -l : lists root directory contents
    -r : lists directory tree contents
    -c : use dircache data (must be used with -l)
    -s : display entries logical block pointer (must be used with -l)
    -m : display file comments, if exists (must be used with -l)

    -v n : mount volume #n instead of default #0 volume

    -p : send extracted files to pipe (unadf -p dump.adf Pics/pic1.gif | xv -)
    -d dir : extract to 'dir' directory
```

### adf_floppy_create

Creates an image of a floppy disk (empty, not formatted). It can create
standard DD (double density) 880K floppy image, HD (high density) 1760K, or
special formats extended number of tracks (like DD with 81-83 tracks).


### adf_floppy_format

Formats the specified floppy disk image file (an ADF, ie. one created with
`adf_floppy_create`), creating on it the Amiga filesystem of the specified
type (in particular: OFS/"Old File System" or FFS/"Fast File System").


### adf_show_metadata

A low-level utility / diagnostic tool, showing metadata about a device / device
image or a file/directory inside the Amiga filesystem. In particular, it shows
contents of Amiga filesystem metadata blocks, it can help understand structure
of Amiga filesystems (for anyone curious...).


## Credits:

- main design and code : Laurent Clevy
- current maintainer, recent core developments (Dec 2022-): Tomasz Wolak
- unadf rewrite (in 2019) : Stuart Caie
- Bug fixes and C++ wrapper : Bjarke Viksoe (adfwrapper.h)
- WinNT native driver : Dan Sutherland and Gary Harris

New versions and contact e-mail can be found at : https://github.com/lclevy/ADFlib

## Past security bugs

Please note that several security issues/bugs has been found in the older (0.7.x)
versions of the ADFlib:
- `CVE-2016-1243` and `CVE-2016-1244`, fixed in
[8e973d7](https://github.com/lclevy/ADFlib/commit/8e973d7b894552c3a3de0ccd2d1e9cb0b8e618dd)),
(found in Debian version `unadf/0.7.11a-3`, fixed in versions `unadf/0.7.11a-4`,
`unadf/0.7.11a-3+deb8u1`). See https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=838248
- Stuart Caie fixed arbitrary directory traversal in
[4ce14b2](https://github.com/lclevy/ADFlib/commit/4ce14b2a8b6db84954cf9705459eafebabecf3e4)
lines 450-455

**Please update to the latest released version where these,
as well as many other things, are fixed.**


## Compilation and installation

See INSTALL file.


## Files

- `AUTHORS` : Contributors
- `README.md` : The file you are reading
- `ChangeLog` : updates in subsequent versions
- `INSTALL` : compilation, testing and installation information
- `TODO` : possible future changes/improvements
- `src/` :	main library files
- `src/win32/` : WinNT native driver (untested!)
- `src/linux/` : Linux native driver (experimental!)
- `src/generic/` : native files templates ("dummy" device)
- `regtests/Boot/` :	Bootblocks that might by used to put on floppy disks
- `doc/` :	The library developer's documentation, man pages for utilities
- `doc/FAQ/` : The Amiga Filesystem explained
- `examples/` : utilities: `unadf`, `adf_floppy_create/format`, `adf_show_metadata`
- `regtests/` : regression tests
- `tests/` : unit tests


## Current status
Most of the code has certain age (the lib was designed and in large part
implemented before the year 2000). Some subsystems have improved, but many
remain untouched for a long time and may not have dedicated tests (meaning:
are not tested).

### Native devices
The library is mostly tested with ADF disk (ie. floppy) images, not with
any native devices. If you do not need them - do not compile them (build
the library with the `generic` (dummy) native device, see INSTALL for details).

If you want really to use and/or tests them, remember that: **the native device
support means accessing physical devices(!)**, please _know what you are doing_
(eg. do not open your `C:\` on windows or `/dev/sda` on Linux and call
a function for creating an Amiga filesystem on it... unless you really want
to reinstall your system and restore your data from a backup...).

Since native devices are not much tested - consider them as testing/experimental
and treat as such (ie. use in a safe environment like a VM).

**YOU HAVE BEEN WARNED**

### The new (completed) file write support
The (fixed) file read support and the (new) file write support are rather
well tested, but still, writing is a new feature so do not experiment on
a unique copy of an ADF image with your precious data. Please do it on a copy
and report if any issues are encountered.

(See also TODO).

## Possible bugs
- in dircache updates


## The API documentation in doc/
... in major part is outdated. The lib underwent many changes and, possibly,
many are to come, no enough time to fully deal with this, too...

Please use it as a reference for the concepts and general ways of use (as those
have not changed) while for the details regarding functions check the header
files, sources in `tests/` and `examples/` to see in details how the current
API can be used.

<!--Please report any bugs or mistakes in the documentation !-->

Have fun anyway !


## Contributing
If you encountered a problem, please review
[the existing issues](https://github.com/lclevy/ADFlib/issues), and,
if the problem you have is not already there, open a new one.

For bugfixes and/or new things - please open a _Pull Request_ to the `devel`
branch (not the `master`).

### Repository structure
Using the code from the repository will be easier understanding that the project
has adapted
[a new branching model](https://nvie.com/posts/a-successful-git-branching-model/)
and so far use the following:
- the `master` branch contains the latest tagged and released code
- the `devel` branch contains development code that was tested and accepted
  for future releases
- other existing branches are new "feature" branches (that may or may not be merged)

Other things of the scheme may be used/adapted as needed (hotfixes for released code
and such, very likely release branch(es) will also appear).


## Projects using ADFlib

[`fuseadf`](https://gitlab.com/t-m/fuseadf) - FUSE-based Linux filesystem allowing
to mount and access ADF images in read/write mode.

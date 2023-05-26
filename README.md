# ADFlib (Amiga Disk File library)

## Introduction

The ADFlib is a free, portable and open implementation of the Amiga filesystem.

Initial release in 1999.

It supports :
- floppy dumps
- multiple partitions harddisk dumps
- UAE hardfiles
- WinNT and Linux devices with the 'native driver'
- mount/unmount/create a device (real one or a dump file),
- mount/unmount/create a volume (partition),
- create/open/close/delete/rename/undel a file,
- read/write bytes from/to a file,
- create/delete/rename/move/undel a directory,
- get directory contents, change current directory, get parent directory
- use dir cache to get directory contents.
- hard- and softlinks for accessing files and directories

It is written in portable C, and support the WinNT platform to access real drives.


## Command-line utilities

The `examples/` directory contains few useful command-line utilities.

Usage info is shown when they are executed without any parameters (see also man pages).


### unADF

unADF is a unzip like utility for .ADF files:

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
- Bug fixes and C++ wrapper : Bjarke Viksoe (adfwrapper.h)
- WinNT native driver : Dan Sutherland and Gary Harris


New versions and contact e-mail can be found at : https://github.com/lclevy/ADFlib

## Security

Please note that CVEs has been found (CVE-2016-1243 and CVE-2016-1244, fixed in
[8e973d7](https://github.com/lclevy/ADFlib/commit/8e973d7b894552c3a3de0ccd2d1e9cb0b8e618dd)),
well ADFlib was designed before year 2000. Found in version Debian unadf/0.7.11a-3,
fixed in versions unadf/0.7.11a-4, unadf/0.7.11a-3+deb8u1.
See https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=838248

Stuart Caie fixed arbitrary directory traversal in
[4ce14b2](https://github.com/lclevy/ADFlib/commit/4ce14b2a8b6db84954cf9705459eafebabecf3e4)
lines 450-455


## Compilation

See INSTALL file.


## Files

- `AUTHORS` : Contributors
- `README.md` : The file you are reading
- `ChangeLog` : updates in subsequent versions
- `src/` :	main library files
- `src/win32/` : WinNT native driver
- `src/linux/` : Linux native driver (experimental!)
- `src/generic/` : native files templates ("dummy" device)
- `boot/` :	Bootblocks that might by used to put on floppy disks
- `doc/` :	The library developer's documentation
- `doc/FAQ/` : The Amiga Filesystem explained
- `examples/` :	utilities: unadf, adf_floppy_create/format, adf_show_metadata


## Features needing testing

### Native driver

(this is an outdated info - to update)

The NATIV_DIR variable is used to choose the (only one) target platform
of the native driver. The default is :
```
NATIV_DIR = ./Generic
```
This one do not give access to any real device. The other one available is
Win32, to access real devices under WinNT.


### Win32DLL

The 'prefix.h' is used to create the Win32 DLL version of the library.
If the WIN32DLL variable is defined in the library code, public functions
are preceded by the '__declspec(dllexport)' directive. If this same
variable is defined, the '__declspec(dllimport)' is put before the functions
prototypes in the 'adflib.h' library include file.


## Current status
Most of the code has certain age. While there where some improvements done,
there is still a long way to go. What you should be aware of:
- it is mostly tested with ADF disk (ie. floppy) images, not native devices
- native device support means accessing physical devices(!), please know what
  you are doing (eg. do not open your `C:\` on windows or `/dev/sda` on Linux
  and call a function for creating an Amiga filesystem on it... unless you
  really want to reinstall your system and restore your data from a backup...)
- also, since native devices are not much tested - consider them as
  testing/experimental and treat as such (ie. use in a safe environment like
  a VM) and if you do not need them, do not use or even compile them with your
  lib (use only "generic")
- the (fixed) file read support and the (new) file write support are rather
  well tested, but still, writing is a new feature so do not experiment on
  a unique copy of an ADF image with your precious data, please do it on a copy
  and report if any issues are encountered

## Possible bugs

- in dircache updates

Please report any bugs or mistakes in the documentation !


Have fun anyway !

# ADFlib (Amiga Disk File library)

## Introduction

The ADFlib is a free, portable and open implementation of the Amiga filesystem.

Initial release in 1999.

It supports :
- floppy dumps
- multiple partitions harddisk dumps
- UAE hardfiles
- WinNT devices with the 'native driver' written by Dan Sutherland
- Linux devices with the 'native driver' written by Tomasz Wolak
- mount/unmount/create a device (real one or a dump file),
- mount/unmount/create a volume (partition),
- create/open/close/delete/rename/undel a file,
- read/write bytes from/to a file,
- create/delete/rename/move/undel a directory,
- get directory contents, change current directory, get parent directory
- use dir cache to get directory contents.


It is written in portable C, and support the WinNT platform to access real drives.


## unADF

unADF is a unzip like for .ADF files :

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


## Credits:

- main design and code : Laurent Clevy
- current maint. and recent fixes (2022): Tomasz Wolak
- Bug fixes and C++ wrapper : Bjarke Viksoe (adfwrapper.h)
- WinNT native driver : Dan Sutherland and Gary Harris


New versions and contact e-mail can be found at : https://github.com/lclevy/ADFlib

## Security

Please note that CVEs has been found (CVE-2016-1243 and CVE-2016-1244, fixed in [8e973d7](https://github.com/lclevy/ADFlib/commit/8e973d7b894552c3a3de0ccd2d1e9cb0b8e618dd)), well ADFlib was designed before year 2000. Found in version Debian unadf/0.7.11a-3, fixed in versions unadf/0.7.11a-4, unadf/0.7.11a-3+deb8u1. See https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=838248

Stuart Caie fixed arbitrary directory traversal in [4ce14b2](https://github.com/lclevy/ADFlib/commit/4ce14b2a8b6db84954cf9705459eafebabecf3e4) lines 450-455


## Compilation

See INSTALL file.


## Files

- AUTHORS : Contributors
- readme.md : The file you are reading
- TODO : Future improvements and bugfixes
- CHANGES : Detailed changes
- src/ :	main library files
- src/win32/ : WinNT native driver
- src/linux/ : Linux native driver (experimental!)
- src/generic/ : native files templates
- boot/ :	Bootblocks that might by used to put on floppy disks
- doc/ :	The library developpers documentation 
- doc/FAQ/ : The Amiga Filesystem explained
- examples/ :	unadf.c


## Features needing testing

### Native driver

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
- native device support means accessing physical devices(!), please know what you are doing
  (eg. do not open your C:\ on windows or /dev/sda on Linux and call a function for creating
  Amiga filesystem on it... unless you really want to reinstall your system and restore your
  data from a backup...)
- also, since native devices are not much tested - consider them as testing/experimental
  and treat as such (ie. use in a safe environment like a VM) and if you do not need them,
  do not use or even compile them with your lib (use only "generic")
- read support is rather well tested
- write support is found to be very buggy(!)
  - works only if you create a new file, write it and close it
  - any reopening and appending or overwriting the data in a file with fail (and may currupt
    your disk/image, so be aware)


## Possible bugs

- write support
- in dircache updates
- lost memory releases


Please report any bugs or mistakes in the documentation !


Have fun anyway !

#!/bin/sh

# Some hosts (Mac homebrew) installs a modern libtoolize as 'glibtoolize'.
# Allow `LIBTOOLIZE' to be set in the environment to allow this.
#if [ "x${LIBTOOLIZE}" = "x" ]
#then
#  LIBTOOLIZE=libtoolize
#fi


# Set libtoolize (or glibtoolize on Darwin, ie. MacOSX)
case `uname` in
       Darwin*)
               LIBTOOLIZE=glibtoolize
               ;;
       *)
               LIBTOOLIZE=libtoolize
esac

$LIBTOOLIZE --copy --force
aclocal
autoconf
autoheader
automake --add-missing


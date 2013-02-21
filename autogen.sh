#!/bin/sh
libtoolize --copy --force
aclocal
autoconf
autoheader
automake --add-missing


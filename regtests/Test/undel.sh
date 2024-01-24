#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- undel"
./undel
rm -v undel-newdev

#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- rename"
./rename
rm -v rename-newdev

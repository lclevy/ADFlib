#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- rename2"
./rename2
rm -v rename2-newdev

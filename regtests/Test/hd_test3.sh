#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- hd_test3"
./hd_test3
rm -v hd_test3-newdev

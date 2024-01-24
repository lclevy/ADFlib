#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- hd_test2"
./hd_test2
rm -v hd_test2-newdev

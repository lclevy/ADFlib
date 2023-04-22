#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- hd_test3"
./hd_test3
rm -v hd_test3-newdev

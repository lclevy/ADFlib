#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- rename"
./rename
rm -v rename-newdev

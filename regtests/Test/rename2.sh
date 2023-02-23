#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- rename2"
./rename2
rm -v rename2-newdev

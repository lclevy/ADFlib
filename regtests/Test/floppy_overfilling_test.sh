#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- floppy_over_filling"
./floppy_overfilling_test
rm -v test.adf

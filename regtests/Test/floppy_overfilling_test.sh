#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- floppy_over_filling"
./floppy_overfilling_test
rm -v test_floppy_overfilling_ofs.adf test_floppy_overfilling_ffs.adf

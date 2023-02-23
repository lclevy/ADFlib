#!/bin/sh

# common settings
. ./common.sh

set -e


#echo "----- fl_test"
./fl_test $FFSDUMP $BOOTBLK
rm -v fl_test-newdev

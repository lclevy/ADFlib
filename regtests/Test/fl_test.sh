#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- fl_test"
./fl_test $FFSDUMP $BOOTBLK
rm -v fl_test-newdev

#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- dir_test"
cat $FFSDUMP >testffs_adf
./dir_test $FFSDUMP
rm -v testffs_adf

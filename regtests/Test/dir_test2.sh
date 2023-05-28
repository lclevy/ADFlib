#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- dir_test2"
cat $FFSDUMP >testffs_adf
./dir_test2 testffs_adf
rm -v testffs_adf

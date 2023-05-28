#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e


#echo "----- del_test"
cat $FFSDUMP >testffs_adf
./del_test testffs_adf
rm -fv testffs_adf

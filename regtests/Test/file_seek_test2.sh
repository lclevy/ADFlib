#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- file_seek_test2"
cat $OFSDUMP >testofs_adf
cat $FFSDUMP >testffs_adf
./file_seek_test2 testofs_adf testffs_adf
rm -v testofs_adf testffs_adf

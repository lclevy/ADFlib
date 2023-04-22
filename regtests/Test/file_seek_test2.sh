#!/bin/sh

# common settings
. ./common.sh

set -e


#echo "----- file_seek_test2"
cp -v $OFSDUMP testofs_adf
cp -v $FFSDUMP testffs_adf
./file_seek_test2 testofs_adf testffs_adf
rm -v testofs_adf testffs_adf

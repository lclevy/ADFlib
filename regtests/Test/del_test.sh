#!/bin/sh

# common settings
. ./common.sh

set -e


#echo "----- del_test"
cp -v $FFSDUMP testffs_adf
./del_test testffs_adf
rm -v testffs_adf

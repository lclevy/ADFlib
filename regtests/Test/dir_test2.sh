#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- dir_test2"
cp -v $FFSDUMP testffs_adf
./dir_test2 testffs_adf
rm -v testffs_adf

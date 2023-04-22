#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- dir_test"
cp -v $FFSDUMP testffs_adf
./dir_test $FFSDUMP
rm -v testffs_adf

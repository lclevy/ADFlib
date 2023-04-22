#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- dir_test_chdir"
cp -v $FFSDUMP testffs_adf
cp -v $LINK_CHAINS_DUMP link_chains_adf
./dir_test_chdir testffs_adf link_chains_adf
rm -v testffs_adf link_chains_adf

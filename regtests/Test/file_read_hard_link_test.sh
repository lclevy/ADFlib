#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- file_read_hard_link_test"
cp -v $FFSDUMP testffs_adf
cp -v $LINK_CHAINS_DUMP link_chains_adf
./file_read_hard_link_test testffs_adf link_chains_adf
rm -v testffs_adf link_chains_adf

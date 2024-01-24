#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- dir_test_chdir"
cat $FFSDUMP >testffs_adf
cat $LINK_CHAINS_DUMP >link_chains_adf
./dir_test_chdir testffs_adf link_chains_adf
rm -v testffs_adf link_chains_adf

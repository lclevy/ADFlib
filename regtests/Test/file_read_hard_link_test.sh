#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- file_read_hard_link_test"
cat $FFSDUMP >testffs_adf
cat $LINK_CHAINS_DUMP >link_chains_adf
./file_read_hard_link_test testffs_adf link_chains_adf
rm -v testffs_adf link_chains_adf

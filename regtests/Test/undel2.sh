#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- undel2"
TEST_FILE=mod.and.distantcall
cat $FFSDUMP >testffs_adf
./undel2 testffs_adf ${TEST_FILE} 886
#diff mod.distant $CHECK/mod.And.DistantCall
rm -v ${TEST_FILE} testffs_adf

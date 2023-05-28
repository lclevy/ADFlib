#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- undel2"
cat $FFSDUMP >testffs_adf
./undel2 testffs_adf
#diff mod.distant $CHECK/mod.And.DistantCall
rm -v mod.distant testffs_adf

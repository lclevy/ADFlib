#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- undel2"
cp -v $FFSDUMP testffs_adf
./undel2 testffs_adf
#diff mod.distant $CHECK/mod.And.DistantCall
rm -v mod.distant testffs_adf

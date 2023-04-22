#!/bin/sh

# common settings
. ./common.sh

set -e


#echo "----- file_test"
./file_test $FFSDUMP $OFSDUMP
#diff mod.distant $CHECK/mod.And.DistantCall
#diff moon_gif $CHECK/MOON.GIF
rm -v mod.distant moon_gif

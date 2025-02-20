#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

DUMP=$DUMPS/g1a30c.adf
TMPDUMP=test_read_bitmap_adf

#echo "----- bitmap_read_segfault_test"
cat $DUMP > $TMPDUMP
./bitmap_read_segfault $TMPDUMP
rm -fv $TMPDUMP

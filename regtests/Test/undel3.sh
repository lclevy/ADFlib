#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- undel3"
cat $OFSDUMP >testofs_adf
./undel3 testofs_adf
#diff moon_gif $CHECK/MOON.GIF
rm -v moon_gif testofs_adf

#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- undel3"
cp -v $OFSDUMP testofs_adf
./undel3 testofs_adf
#diff moon_gif $CHECK/MOON.GIF
rm -v moon_gif testofs_adf

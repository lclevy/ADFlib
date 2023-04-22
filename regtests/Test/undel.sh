#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- undel"
./undel
rm -v undel-newdev

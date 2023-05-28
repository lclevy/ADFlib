#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

#echo "----- bootdisk"
./bootdisk $BOOTBLK
rm -v bootdisk-newdev

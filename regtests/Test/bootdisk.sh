#!/bin/sh

# common settings
. ./common.sh

set -e

#echo "----- bootdisk"
./bootdisk $BOOTBLK
rm -v bootdisk-newdev

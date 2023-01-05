#!/bin/sh


DUMPS=../Dumps
FFSDUMP=$DUMPS/testffs.adf
OFSDUMP=$DUMPS/testofs.adf
HDDUMP=$DUMPS/testhd.adf

BOOTDIR=../Boot
BOOTBLK=$BOOTDIR/stdboot3.bbk

CHECK=../Check

set -e

#echo "----- hd_test"
#./hd_test /home/root/hd.adf /home/root/idh2.adf

echo "----- hd_test2"
./hd_test2
rm -v newdev

echo "----- hd_test3"
./hd_test3
rm -v newdev

#echo "----- hardfile"
#./hardfile /home/root/hardfile.hdf

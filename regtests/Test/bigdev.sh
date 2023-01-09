#!/bin/sh



# autotools uses $srcdir
if [ "x$srcdir" != "x" ]
then
    TESTS_DIR=${srcdir}/regtests
else
    TESTS_DIR=".."
fi

DUMPS=${TESTS_DIR}/Dumps

FFSDUMP=$DUMPS/testffs.adf
OFSDUMP=$DUMPS/testofs.adf
HDDUMP=$DUMPS/testhd.adf

BOOTDIR=${TESTS_DIR}/Boot
BOOTBLK=$BOOTDIR/stdboot3.bbk

CHECK=${TESTS_DIR}/Check

set -e

#echo "----- hd_test"
#./hd_test /home/root/hd.adf /home/root/idh2.adf

echo "----- hd_test2"
./hd_test2
rm -v hd_test2-newdev

echo "----- hd_test3"
./hd_test3
rm -v hd_test3-newdev

#echo "----- hardfile"
#./hardfile /home/root/hardfile.hdf

#!/bin/sh
# floppy test

PATH=.:$PATH

DUMPS=../Dumps
FFSDUMP=$DUMPS/testffs.adf
OFSDUMP=$DUMPS/testofs.adf
HDDUMP=$DUMPS/testhd.adf
LINK_CHAINS_DUMP=$DUMPS/test_link_chains.adf

BOOTDIR=../Boot
BOOTBLK=$BOOTDIR/stdboot3.bbk

CHECK=../Check

set -e

bootdisk $BOOTBLK
rm newdev
echo "-----"

cp $FFSDUMP testffs_adf
del_test testffs_adf
rm testffs_adf
echo "-----"

dir_test $FFSDUMP
echo "-----"

cp $FFSDUMP testffs_adf
dir_test2 testffs_adf
rm testffs_adf
echo "-----"

echo "Executing dir_test_chdir..."
cp $FFSDUMP testffs_adf
cp $LINK_CHAINS_DUMP link_chains_adf
dir_test_chdir testffs_adf link_chains_adf
rm testffs_adf link_chains_adf
echo "-----"


fl_test $FFSDUMP $BOOTBLK
rm newdev
echo "-----"

fl_test2 $HDDUMP
echo "-----"

file_test $FFSDUMP $OFSDUMP
#diff mod.distant $CHECK/mod.And.DistantCall
#diff moon_gif $CHECK/MOON.GIF
rm mod.distant moon_gif
echo "-----"

#cp $FFSDUMP testffs_adf
#file_test2 testffs_adf $CHECK/MOON.GIF
#diff moon__gif $CHECK/MOON.GIF
#rm moon__gif
#rm testffs_adf
echo "-----"

#cp $OFSDUMP testofs_adf
#file_test3 testofs_adf $CHECK/MOON.GIF
#diff moon__gif $CHECK/MOON.GIF
#rm moon__gif
#rm testofs_adf
echo "-----"

echo "Executing file_seek_test..."
cp $OFSDUMP testofs_adf
cp $FFSDUMP testffs_adf
file_seek_test testofs_adf testffs_adf
rm testofs_adf testffs_adf
echo "-----"

echo "Executing file_seek_test2..."
cp $OFSDUMP testofs_adf
cp $FFSDUMP testffs_adf
file_seek_test2 testofs_adf testffs_adf
rm testofs_adf testffs_adf
echo "-----"

echo "Executing file_read_hard_link_test..."
cp $FFSDUMP testffs_adf
cp $LINK_CHAINS_DUMP link_chains_adf
file_read_hard_link_test testffs_adf link_chains_adf
rm testffs_adf link_chains_adf
echo "-----"


rename
rm newdev
echo "-----"

rename2
rm newdev

undel
rm newdev
echo "-----"

cp $FFSDUMP testffs_adf
undel2 testffs_adf
#diff mod.distant $CHECK/mod.And.DistantCall
rm mod.distant testffs_adf
echo "-----"

cp $OFSDUMP testofs_adf
undel3 testofs_adf
#diff moon_gif $CHECK/MOON.GIF
rm moon_gif testofs_adf
echo "-----"


echo "Running over-filling test..."
floppy_overfilling_test
rm test.adf
echo "-----"

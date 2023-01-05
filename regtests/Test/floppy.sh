#!/bin/sh
# floppy test

DUMPS=../Dumps
FFSDUMP=$DUMPS/testffs.adf
OFSDUMP=$DUMPS/testofs.adf
HDDUMP=$DUMPS/testhd.adf
LINK_CHAINS_DUMP=$DUMPS/test_link_chains.adf

BOOTDIR=../Boot
BOOTBLK=$BOOTDIR/stdboot3.bbk

CHECK=../Check

set -e

echo "----- bootdisk"
./bootdisk $BOOTBLK
rm -v newdev

echo "----- del_test"
cp -v $FFSDUMP testffs_adf
./del_test testffs_adf
rm -v testffs_adf

echo "----- dir_test"
cp -v $FFSDUMP testffs_adf
./dir_test $FFSDUMP
rm -v testffs_adf

echo "----- dir_test2"
cp -v $FFSDUMP testffs_adf
./dir_test2 testffs_adf
rm -v testffs_adf

echo "----- dir_test_chdir"
cp -v $FFSDUMP testffs_adf
cp -v $LINK_CHAINS_DUMP link_chains_adf
./dir_test_chdir testffs_adf link_chains_adf
rm -v testffs_adf link_chains_adf

echo "----- fl_test"
./fl_test $FFSDUMP $BOOTBLK
rm -v newdev

echo "----- fl_test2"
./fl_test2 $HDDUMP

echo "----- file_test"
./file_test $FFSDUMP $OFSDUMP
#diff mod.distant $CHECK/mod.And.DistantCall
#diff moon_gif $CHECK/MOON.GIF
rm -v mod.distant moon_gif

#echo "----- file_test2"
#cp -v $FFSDUMP testffs_adf
#./file_test2 testffs_adf $CHECK/MOON.GIF
#diff moon__gif $CHECK/MOON.GIF
#rm -v moon__gif
#rm -v testffs_adf

#echo "----- file_test3"
#cp -v $OFSDUMP testofs_adf
#./file_test3 testofs_adf $CHECK/MOON.GIF
#diff moon__gif $CHECK/MOON.GIF
#rm -v moon__gif
#rm -v testofs_adf

echo "----- file_seek_test"
cp -v $OFSDUMP testofs_adf
cp -v $FFSDUMP testffs_adf
./file_seek_test testofs_adf testffs_adf
rm -v testofs_adf testffs_adf

echo "----- file_seek_test2"
cp -v $OFSDUMP testofs_adf
cp -v $FFSDUMP testffs_adf
./file_seek_test2 testofs_adf testffs_adf
rm -v testofs_adf testffs_adf

echo "----- file_read_hard_link_test"
cp -v $FFSDUMP testffs_adf
cp -v $LINK_CHAINS_DUMP link_chains_adf
./file_read_hard_link_test testffs_adf link_chains_adf
rm -v testffs_adf link_chains_adf

echo "----- rename"
./rename
rm -v newdev

echo "----- rename2"
./rename2
rm -v newdev

echo "----- undel"
./undel
rm -v newdev

echo "----- undel2"
cp -v $FFSDUMP testffs_adf
./undel2 testffs_adf
#diff mod.distant $CHECK/mod.And.DistantCall
rm -v mod.distant testffs_adf

echo "----- undel3"
cp -v $OFSDUMP testofs_adf
./undel3 testofs_adf
#diff moon_gif $CHECK/MOON.GIF
rm -v moon_gif testofs_adf

echo "----- floppy_over_filling"
./floppy_overfilling_test
rm -v test.adf

echo "-----"

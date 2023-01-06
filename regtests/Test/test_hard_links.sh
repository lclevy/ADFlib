#!/bin/sh
# hard links test

#PATH=.:$PATH


TESTS_DIR=".."

# autotools
if [ "x$srcdir" != "x" ]
then
    TESTS_DIR=${srcdir}/${TESTS_DIR}
fi

DUMPS=${TESTS_DIR}/Dumps

FFSDUMP=$DUMPS/testffs.adf
OFSDUMP=$DUMPS/testofs.adf
HDDUMP=$DUMPS/testhd.adf
LINK_CHAINS_DUMP=$DUMPS/test_link_chains.adf

set -e

echo "Executing file_read_hard_link_test..."
cp -v $FFSDUMP testffs_adf
cp -v $LINK_CHAINS_DUMP link_chains_adf
./file_read_hard_link_test testffs_adf link_chains_adf
rm -v testffs_adf link_chains_adf
echo "-----"

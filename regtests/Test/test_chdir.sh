#!/bin/sh
#
# chdir test
#

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

echo "Executing dir_test_chdir..."
cp $FFSDUMP testffs_adf
cp $LINK_CHAINS_DUMP link_chains_adf
#dir_test_chdir testffs_adf link_chains_adf
./dir_test_chdir testffs_adf link_chains_adf
rm testffs_adf link_chains_adf
echo "-----"

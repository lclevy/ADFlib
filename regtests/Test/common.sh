
#
# common settings to source in each of the tests
#


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

BOOTDIR=${TESTS_DIR}/Boot
BOOTBLK=$BOOTDIR/stdboot3.bbk

CHECK=${TESTS_DIR}/Check

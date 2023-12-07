#!/bin/bash
#
# A simple test of command-line utilities.
#
# It tries to execute them with some parameters
# and shows the outputs (it does no further checks),
# fails if the execution is not possible.
#

set -e

if [ $# -ne 1 ]
then
    echo 2>&1 "Usage: $0 <bin install path>"
    exit 1
fi

PATH=`pwd`/$1:$PATH
EXAMPLES_TEST_PATH=`dirname $0`
TEST_ADF=$EXAMPLES_TEST_PATH/arccsh.adf

if [ ! -f $EXAMPLES_TEST_PATH/arccsh.adf ]
then
    echo "Test data not available ( $EXAMPLES_TEST_PATH/arccsh.adf )."
    exit 1
fi

#[ -n "$srcdir" ] && TEST_ADF="$srcdir/arccsh.adf"

run_cmd()
{
    echo "============================================================="
    #stdbuf -oL
    if [ ${OSTYPE} == "msys" ]
    then
	EXEC=${1}.exe
    else
	EXEC=${1}
    fi
    shift
    echo "executing:  ${EXEC} $@"
    #${EXAMPLES_PATH}/${EXEC} $@
    ${EXEC} $@
}

CMDS[0]="unadf -r $TEST_ADF"
CMDS[1]="adf_show_metadata $TEST_ADF"
CMDS[2]="adf_show_metadata $TEST_ADF CSH"
CMDS[3]="adf_show_metadata $TEST_ADF c/"
CMDS[4]="adf_show_metadata $TEST_ADF l"
CMDS[5]="adf_floppy_create testflopdd1.adf dd"
CMDS[6]="adf_floppy_format testflopdd1.adf TestFlopDD1 1"
CMDS[7]="adf_show_metadata testflopdd1.adf"
CMDS[8]="rm -v testflopdd1.adf"
CMDS[9]="adf_bitmap show $TEST_ADF"

for CMD in "${CMDS[@]}"
do
    run_cmd ${CMD}
done

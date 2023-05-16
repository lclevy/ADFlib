#!/bin/sh
#
# floppy test
#

#set -e

NFAILED=0
FAILED=

case `uname` in
    Darwin*)
	STDBUF=gstdbuf
	;;
    *)
	STDBUF=stdbuf
esac

for A_TEST in \
    bootdisk.sh \
    cache_crash.sh \
    del_test.sh \
    dir_test.sh \
    dir_test2.sh \
    dir_test_chdir.sh \
    fl_test.sh \
    fl_test2.sh \
    file_test.sh \
    file_test2.sh \
    file_test3.sh \
    file_seek_test.sh \
    file_seek_test2.sh \
    file_seek_after_write \
    file_read_hard_link_test.sh \
    rename.sh \
    undel.sh \
    undel2.sh \
    undel3.sh \
    floppy_overfilling_test.sh
do
    echo "---------------------------------------------------"
    echo "    Executing: ${A_TEST}"
    echo "---------------------------------------------------"
    ${STDBUF} -oL -eL ./${A_TEST}
    RESULT=${?}
    #echo "Result: ${RESULT}"
    if [ ${RESULT} -ne 0 ]
    then
	NFAILED=$(( ${NFAILED} + 1 ))
	FAILED="${FAILED}\n  ${A_TEST}  (result: ${RESULT})"
    fi    
done

echo "---------------------------------------------------"
echo "${NFAILED} tests failed: ${FAILED}"
echo "---------------------------------------------------"

[ ${NFAILED} -eq 0 ]

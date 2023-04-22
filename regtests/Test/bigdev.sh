#!/bin/sh
#
# bigdev / harddisk (fileimage) tests
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
   ./hd_test.sh \
   ./hd_test2.sh \
   ./hd_test3.sh \
   ./hardfile.sh
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

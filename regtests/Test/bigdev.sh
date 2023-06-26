#!/bin/sh
#
# bigdev / harddisk (fileimage) tests
#

#set -e

NFAILED=0
FAILED=

for A_TEST in \
   ./hd_test.sh \
   ./hd_test2.sh \
   ./hd_test3.sh \
   ./hardfile.sh
do
    echo "---------------------------------------------------"
    echo "    Executing: ${A_TEST}"
    echo "---------------------------------------------------"
    if [ -n "${srcdir}" -a -x "${srcdir}/${A_TEST}" ]; then
        ${srcdir}/${A_TEST}
    else
        ./${A_TEST}
    fi
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

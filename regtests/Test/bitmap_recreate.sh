#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

echo "----- bitmap_recreate"

for DUMP_ORIG in $DUMPS/*.adf
do
    DUMP=`basename "${DUMP_ORIG}"`

    # exclude failing
    [ "$DUMP" = "cache_crash.adf" ] && continue;
    [ "$DUMP" = "testffs.adf" ]     && continue;     
    [ "$DUMP" = "testhd.adf" ]      && continue;

    echo "Copying ${DUMP_ORIG} to ${DUMP}"
    cat "${DUMP_ORIG}" > "${DUMP}"
    ./bitmap_recreate "${DUMP}"
    rm -fv "${DUMP}"
done

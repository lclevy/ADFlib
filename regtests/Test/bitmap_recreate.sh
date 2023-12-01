#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

echo "----- bitmap_recreate"

for DUMP_ORIG in `ls $DUMPS/*.adf`
do
    DUMP=`basename $DUMP_ORIG`

    # exclude failing
    [ "$DUMP" = "cache_crash.adf" ] && continue;
    [ "$DUMP" = "testffs.adf" ]     && continue;     
    [ "$DUMP" = "testhd.adf" ]      && continue;

    #DUMP_UPDATED=${DUMP}.bitmap_update.adf
    echo "Copying $DUMP_ORIG to $DUMP $DUMP_UPDATED"
    #cat $DUMP_ORIG | tee $DUMP > $DUMP_UPDATED
    cat $DUMP_ORIG > $DUMP

    ./bitmap_recreate $DUMP

    #cmp -l ${DUMP_COPY} $DUMP_UPDATED

    #rm -fv $DUMP $DUMP_UPDATED
    rm -fv $DUMP
done

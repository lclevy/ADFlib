#!/bin/sh
# Usage: test-all-examples.sh <install path>
#  e.g.: test-all-examples.sh /usr/local/bin
#        test-all-examples.sh .
set -e

if [ $# -ne 1 ]; then
    echo 2>&1 "Usage: $0 <install path>"
    exit 1
fi

basedir=`dirname "$0"`
cmd_path="$1" "$basedir/adf-floppy-test.sh"
cmd_path="$1" "$basedir/adf-show-metadata-test.sh"
cmd_path="$1" "$basedir/unadf-test.sh"


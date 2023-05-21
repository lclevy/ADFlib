#!/bin/sh

# common settings
. "`dirname $0`/common.sh"

set -e

./cache_crash $DUMPS/cache_crash.adf

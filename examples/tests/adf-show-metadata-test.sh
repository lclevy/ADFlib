#!/bin/sh
basedir=`dirname "$0"`
. $basedir/common.sh

adf_show_metadata=`get_test_cmd adf_show_metadata`

$adf_show_metadata "$basedir/arccsh.adf" >$actual
compare_with "adf show metadata device" adf-show-metadata-test_1

$adf_show_metadata "$basedir/arccsh.adf" 0 >$actual
compare_with "adf show metadata volume 0" adf-show-metadata-test_2

$adf_show_metadata "$basedir/arccsh.adf" 0 CSH >$actual
compare_with "adf show metadata file CHS" adf-show-metadata-test_3

$adf_show_metadata "$basedir/arccsh.adf" 0 c/ >$actual
compare_with "adf show metadata directory c/" adf-show-metadata-test_4

$adf_show_metadata "$basedir/arccsh.adf" 0 l >$actual
compare_with "adf show metadata directory l" adf-show-metadata-test_5

read status < $status && test "x$status" = xsuccess

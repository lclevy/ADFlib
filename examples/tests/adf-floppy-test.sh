#!/bin/sh
basedir=`dirname "$0"`
. $basedir/common.sh

adf_floppy_create=`get_test_cmd adf_floppy_create`
adf_floppy_format=`get_test_cmd adf_floppy_format`
adf_show_metadata=`get_test_cmd adf_show_metadata`

$adf_floppy_create $tmpdir/testflopdd1.adf dd >$actual
compare_with "adf floppy create dd" adf-floppy-test_1

$adf_floppy_format $tmpdir/testflopdd1.adf -t 1 -l TestFlopDD1 -v >$actual 2>/dev/null
compare_with "adf floppy format dd" adf-floppy-test_2

$adf_show_metadata $tmpdir/testflopdd1.adf >$actual
compare_with "adf show metadata dd floppy dev" adf-floppy-test_3

$adf_show_metadata $tmpdir/testflopdd1.adf 0 | grep -v \
  -e Created: -e 'Last access:' -e checkSum: -e calculated -e days: \
  -e mins: -e ticks: -e coDays: -e coMins -e coTicks >$actual
compare_with "adf show metadata dd floppy volume" adf-floppy-test_4

read status < $status && test "x$status" = xsuccess

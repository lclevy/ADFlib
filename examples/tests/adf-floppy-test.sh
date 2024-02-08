#!/bin/sh
basedir=`dirname "$0"`
. $basedir/common.sh

adf_floppy_create=`get_test_cmd adf_floppy_create`
adf_floppy_format=`get_test_cmd adf_floppy_format`
adf_show_metadata=`get_test_cmd adf_show_metadata`

$adf_floppy_create $tmpdir/testflopdd1.adf dd >$actual
compare_with <<EOF
Creating floppy disk image: '$tmpdir/testflopdd1.adf'

ADF device info:
  Type:		floppy dd
  Driver:	dump
  Geometry:
    Cylinders	80
    Heads	2
    Sectors	11

  Volumes (0):
   idx  first bl.     last bl.    name

Done!
EOF

$adf_floppy_format $tmpdir/testflopdd1.adf TestFlopDD1 1 >$actual 2>/dev/null
compare_with <<EOF

ADF device info:
  Type:		floppy dd
  Driver:	dump
  Geometry:
    Cylinders	80
    Heads	2
    Sectors	11

  Volumes (0):
   idx  first bl.     last bl.    name

Formatting floppy (DD) disk '$tmpdir/testflopdd1.adf'...
Done!
EOF

$adf_show_metadata $tmpdir/testflopdd1.adf | grep -v \
  -e Created: -e 'Last access:' -e checkSum: -e calculated -e days: \
  -e mins: -e ticks: -e coDays: -e coMins -e coTicks >$actual
compare_with <<EOF

Opening image/device:	'$tmpdir/testflopdd1.adf'
Mounted volume:		0

ADF device info:
  Type:		floppy dd
  Driver:	dump
  Geometry:
    Cylinders	80
    Heads	2
    Sectors	11

  Volumes (1):
   idx  first bl.     last bl.    name
     0          0         1759    "TestFlopDD1"    mounted


ADF volume info:
  Name:		TestFlopDD1                   
  Type:		Floppy Double Density, 880 KBytes
  Filesystem:	FFS  
  Free blocks:	1756
  R/W:		Read only
		1/01/1978 0:00:00

Bootblock:
  dosType:	DOS. (0x1)
  rootBlock:	0x0 (0)

Root block sector:	880

Rootblock:
  0x000  type:		0x2		2
  0x004  headerKey:	0x0		0
  0x008  highSeq:	0x0		0
  0x00c  hashTableSize:	0x48		72
  0x010  firstData:	0x0		0
  0x018  hashTable [ 72 ]:	(see below)
  0x138  bmFlag:	0xffffffff
  0x13c  bmPages[ 25 ]:		(see below)
  0x1a0  bmExt:		0x0
  0x1a4  cDays:		0x0		0
  0x1a8  cMins:		0x0		0
  0x1ac  cTicks:	0x0		0
  0x1b0  nameLen:	0xb		11
  0x1b1  diskName:	TestFlopDD1
  0x1d0  r2[8]:			(see below)
  0x1f0  nextSameHash:	0x0		0
  0x1f4  parent:	0x0		0
  0x1f8  extension:	0x0		0
  0x1fc  secType:	0x1		1

Hashtable (non-zero):

Bitmap block pointers (bmPages) (non-zero):
  bmpages [  0 ]:		0x371		881
EOF

read status < $status && test "x$status" = xsuccess

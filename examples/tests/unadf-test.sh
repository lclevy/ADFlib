#!/bin/sh
set -x
basedir=`dirname "$0"`
. $basedir/common.sh

unadf=`get_test_cmd unadf`

# -l (list root directory) option
$unadf -l "$basedir/arccsh.adf" >$actual 2>/dev/null
compare_with <<EOF
Device : Floppy DD. Cylinders = 80, Heads = 2, Sectors = 11. Volumes = 1
Volume : Floppy 880 KBytes, "cshell" between sectors [0-1759]. OFS. Filled at 75.2%.
         1999/07/30  22:35:01  c/
         1999/07/30  22:14:46  l/
         1999/07/04  14:24:24  devs/
         1999/07/30  22:33:56  s/
         1999/07/04  14:24:31  libs/
 130156  1996/08/03   0:55:04  CSH
   1136  1992/09/02  11:51:33  LoadWB
    232  1992/09/02  11:51:33  system-configuration
EOF

# -r (list entire disk) option
$unadf -r "$basedir/arccsh.adf" >$actual 2>/dev/null
compare_with <<EOF
Device : Floppy DD. Cylinders = 80, Heads = 2, Sectors = 11. Volumes = 1
Volume : Floppy 880 KBytes, "cshell" between sectors [0-1759]. OFS. Filled at 75.2%.
         1999/07/30  22:35:01  c/
  12896  1992/09/02  11:51:33  c/Format
   6880  1994/02/18   9:56:04  c/Mount
  72832  1993/09/21  23:03:34  c/Zip
   1496  1992/09/02  11:51:33  c/Type
  27960  1992/06/28  18:25:02  c/DMS
    241  1999/07/30  22:34:59  c/cmd.txt
  37068  1995/08/23  22:16:56  c/UNLZX
    584  1992/09/02  11:51:33  c/Relabel
   1980  1992/09/02  11:51:33  c/Info
   3652  1992/09/02  11:51:33  c/CPU
   3220  1992/09/02  11:51:33  c/Assign
  80764  1999/03/24  19:44:00  c/Deksid
   5108  1992/09/02  11:51:33  c/List
  12020  1997/07/08  22:09:41  c/transdisk
  54600  1978/01/01   1:07:44  c/LhA
   1136  1992/09/02  11:51:33  c/LoadWB
  83764  1995/08/23  22:15:52  c/LZX
         1999/07/30  22:14:46  l/
    256  1997/12/13  19:37:36  l/LZX.Keyfile
         1999/07/04  14:24:24  devs/
         1999/07/04  17:33:11  devs/DOSDrivers/
    554  1995/01/17  20:10:29  devs/DOSDrivers/SD0.info
    666  1994/01/29  18:18:36  devs/DOSDrivers/SD0
   2480  1994/08/31  16:37:28  devs/statram.device
    232  1992/11/20  12:00:00  devs/SyStEm-CoNfIgUrAtIoN
         1999/07/30  22:33:56  s/
     32  1999/07/04  17:37:30  s/.cshrc
    391  1999/07/30  22:33:29  s/aliases
     46  1999/03/24  19:57:50  s/.login
     67  1999/07/04  17:37:56  s/startup-sequence
         1999/07/04  14:24:31  libs/
  12436  1993/02/07  19:08:56  libs/arp.library
  40452  1992/09/02  11:51:33  libs/asl.library
  15340  1992/09/02  11:51:33  libs/diskfont.library
 130156  1996/08/03   0:55:04  CSH
   1136  1992/09/02  11:51:33  LoadWB
    232  1992/09/02  11:51:33  system-configuration
EOF

# -s (show logical block pointer) option
$unadf -ls "$basedir/arccsh.adf" >$actual 2>/dev/null
compare_with <<EOF
Device : Floppy DD. Cylinders = 80, Heads = 2, Sectors = 11. Volumes = 1
Volume : Floppy 880 KBytes, "cshell" between sectors [0-1759]. OFS. Filled at 75.2%.
         1999/07/30  22:35:01  000882  c/
         1999/07/30  22:14:46  001012  l/
         1999/07/04  14:24:24  001746  devs/
         1999/07/30  22:33:56  000887  s/
         1999/07/04  14:24:31  001363  libs/
 130156  1996/08/03   0:55:04  001014  CSH
   1136  1992/09/02  11:51:33  000883  LoadWB
    232  1992/09/02  11:51:33  000888  system-configuration
EOF

# TODO -c (use dircache data) option
# TODO -m (display file comments) option
# TODO -v (mount different volume) option

# -d (extract to dir)
$unadf -d $tmpdir/x "$basedir/arccsh.adf" >$actual 2>/dev/null
compare_with <<EOF
Device : Floppy DD. Cylinders = 80, Heads = 2, Sectors = 11. Volumes = 1
Volume : Floppy 880 KBytes, "cshell" between sectors [0-1759]. OFS. Filled at 75.2%.
x - $tmpdir/x/c/
x - $tmpdir/x/c/Format
x - $tmpdir/x/c/Mount
x - $tmpdir/x/c/Zip
x - $tmpdir/x/c/Type
x - $tmpdir/x/c/DMS
x - $tmpdir/x/c/cmd.txt
x - $tmpdir/x/c/UNLZX
x - $tmpdir/x/c/Relabel
x - $tmpdir/x/c/Info
x - $tmpdir/x/c/CPU
x - $tmpdir/x/c/Assign
x - $tmpdir/x/c/Deksid
x - $tmpdir/x/c/List
x - $tmpdir/x/c/transdisk
x - $tmpdir/x/c/LhA
x - $tmpdir/x/c/LoadWB
x - $tmpdir/x/c/LZX
x - $tmpdir/x/l/
x - $tmpdir/x/l/LZX.Keyfile
x - $tmpdir/x/devs/
x - $tmpdir/x/devs/DOSDrivers/
x - $tmpdir/x/devs/DOSDrivers/SD0.info
x - $tmpdir/x/devs/DOSDrivers/SD0
x - $tmpdir/x/devs/statram.device
x - $tmpdir/x/devs/SyStEm-CoNfIgUrAtIoN
x - $tmpdir/x/s/
x - $tmpdir/x/s/.cshrc
x - $tmpdir/x/s/aliases
x - $tmpdir/x/s/.login
x - $tmpdir/x/s/startup-sequence
x - $tmpdir/x/libs/
x - $tmpdir/x/libs/arp.library
x - $tmpdir/x/libs/asl.library
x - $tmpdir/x/libs/diskfont.library
x - $tmpdir/x/CSH
x - $tmpdir/x/LoadWB
x - $tmpdir/x/system-configuration
EOF

# check permissions were set on extracted files
[ -x $tmpdir/x/CSH ] || echo failed >$status
[ -x $tmpdir/x/c/LZX ] || echo failed >$status

# -d (extract to dir) with specific files, not all their original case
$unadf -d $tmpdir/x "$basedir/arccsh.adf" csh s/startup-sequence devs/system-configuration >$actual 2>/dev/null
compare_with <<EOF
Device : Floppy DD. Cylinders = 80, Heads = 2, Sectors = 11. Volumes = 1
Volume : Floppy 880 KBytes, "cshell" between sectors [0-1759]. OFS. Filled at 75.2%.
x - $tmpdir/x/csh
x - $tmpdir/x/s/startup-sequence
x - $tmpdir/x/devs/system-configuration
EOF

# TODO check permissions were set (bug: currently they aren't)

# -p (extract to pipe) option
$unadf -p "$basedir/arccsh.adf" s/startup-sequence >$actual 2>/dev/null
compare_with <<EOF
c:assign ENV: ram:
path C: ram: add
mount sd0:
c:loadwb
csh
endcli
EOF

# -w (mangle win32 filenames) option
$unadf -d $tmpdir/x -w "$basedir/win32-names.adf" >$actual 2>/dev/null
compare_with <<EOF
Device : Floppy DD. Cylinders = 80, Heads = 2, Sectors = 11. Volumes = 1
Volume : Floppy 880 KBytes, "win32-names" between sectors [0-1759]. FFS DIRCACHE. Filled at 5.0%.
x - $tmpdir/x/win32-names.sh
x - $tmpdir/x/forbidden-files/
x - $tmpdir/x/forbidden-files/com6_.txt
x - $tmpdir/x/forbidden-files/lpt5_.txt
x - $tmpdir/x/forbidden-files/com4_.txt
x - $tmpdir/x/forbidden-files/lpt3_.txt
x - $tmpdir/x/forbidden-files/CON_
x - $tmpdir/x/forbidden-files/nul_.txt
x - $tmpdir/x/forbidden-files/com2_.txt
x - $tmpdir/x/forbidden-files/lpt1_.txt
x - $tmpdir/x/forbidden-files/prn_.txt
x - $tmpdir/x/forbidden-files/LPT1_
x - $tmpdir/x/forbidden-files/LPT2_
x - $tmpdir/x/forbidden-files/lpt8_.txt
x - $tmpdir/x/forbidden-files/LPT3_
x - $tmpdir/x/forbidden-files/LPT4_
x - $tmpdir/x/forbidden-files/LPT5_
x - $tmpdir/x/forbidden-files/LPT6_
x - $tmpdir/x/forbidden-files/LPT7_
x - $tmpdir/x/forbidden-files/LPT8_
x - $tmpdir/x/forbidden-files/com7_.txt
x - $tmpdir/x/forbidden-files/lpt6_.txt
x - $tmpdir/x/forbidden-files/LPT9_
x - $tmpdir/x/forbidden-files/NUL_
x - $tmpdir/x/forbidden-files/com5_.txt
x - $tmpdir/x/forbidden-files/lpt4_.txt
x - $tmpdir/x/forbidden-files/COM1_
x - $tmpdir/x/forbidden-files/COM2_
x - $tmpdir/x/forbidden-files/con_.txt
x - $tmpdir/x/forbidden-files/COM3_
x - $tmpdir/x/forbidden-files/COM4_
x - $tmpdir/x/forbidden-files/COM5_
x - $tmpdir/x/forbidden-files/com3_.txt
x - $tmpdir/x/forbidden-files/lpt2_.txt
x - $tmpdir/x/forbidden-files/AUX_
x - $tmpdir/x/forbidden-files/COM6_
x - $tmpdir/x/forbidden-files/COM7_
x - $tmpdir/x/forbidden-files/COM8_
x - $tmpdir/x/forbidden-files/COM9_
x - $tmpdir/x/forbidden-files/com1_.txt
x - $tmpdir/x/forbidden-files/PRN_
x - $tmpdir/x/forbidden-files/aux_.txt
x - $tmpdir/x/forbidden-files/com9_.txt
x - $tmpdir/x/forbidden-files/lpt9_.txt
x - $tmpdir/x/forbidden-files/com8_.txt
x - $tmpdir/x/forbidden-files/lpt7_.txt
x - $tmpdir/x/not-forbidden/
x - $tmpdir/x/not-forbidden/auxx.txt
x - $tmpdir/x/not-forbidden/prnn.txt
x - $tmpdir/x/not-forbidden/CONN
x - $tmpdir/x/not-forbidden/NULL
x - $tmpdir/x/not-forbidden/null.txt
x - $tmpdir/x/not-forbidden/AUXX
x - $tmpdir/x/not-forbidden/LPT11
x - $tmpdir/x/not-forbidden/lpt11.txt
x - $tmpdir/x/not-forbidden/PRNN
x - $tmpdir/x/not-forbidden/conn.txt
x - $tmpdir/x/forbidden-suffixes/
x - $tmpdir/x/forbidden-suffixes/test_______
x - $tmpdir/x/forbidden-suffixes/______
x - $tmpdir/x/forbidden-suffixes/R.E.M_
x - $tmpdir/x/forbidden-suffixes/   hello___
x - $tmpdir/x/forbidden-chars/
x - $tmpdir/x/forbidden-chars/test_test_test.txt
x - $tmpdir/x/forbidden-chars/test_test_test.txt
x - $tmpdir/x/forbidden-chars/test_test_test.txt
x - $tmpdir/x/forbidden-chars/test_test_test.txt
x - $tmpdir/x/forbidden-chars/test_test_test.txt
x - $tmpdir/x/forbidden-chars/test_test_test.txt
x - $tmpdir/x/forbidden-chars/test_test_test.txt
EOF

# confirm the mangling (-w) only occurs on extraction
$unadf -r -w "$basedir/win32-names.adf" >$actual 2>/dev/null
compare_with <<EOF
Device : Floppy DD. Cylinders = 80, Heads = 2, Sectors = 11. Volumes = 1
Volume : Floppy 880 KBytes, "win32-names" between sectors [0-1759]. FFS DIRCACHE. Filled at 5.0%.
   3398  2023/05/26  11:43:32  win32-names.sh
         2023/05/26  11:43:32  forbidden-files/
      0  2023/05/26  11:43:32  forbidden-files/com6.txt
      0  2023/05/26  11:43:32  forbidden-files/lpt5.txt
      0  2023/05/26  11:43:32  forbidden-files/com4.txt
      0  2023/05/26  11:43:32  forbidden-files/lpt3.txt
      0  2023/05/26  11:43:32  forbidden-files/CON
      0  2023/05/26  11:43:32  forbidden-files/nul.txt
      0  2023/05/26  11:43:32  forbidden-files/com2.txt
      0  2023/05/26  11:43:32  forbidden-files/lpt1.txt
      0  2023/05/26  11:43:32  forbidden-files/prn.txt
      0  2023/05/26  11:43:32  forbidden-files/LPT1
      0  2023/05/26  11:43:32  forbidden-files/LPT2
      0  2023/05/26  11:43:32  forbidden-files/lpt8.txt
      0  2023/05/26  11:43:32  forbidden-files/LPT3
      0  2023/05/26  11:43:32  forbidden-files/LPT4
      0  2023/05/26  11:43:32  forbidden-files/LPT5
      0  2023/05/26  11:43:32  forbidden-files/LPT6
      0  2023/05/26  11:43:32  forbidden-files/LPT7
      0  2023/05/26  11:43:32  forbidden-files/LPT8
      0  2023/05/26  11:43:32  forbidden-files/com7.txt
      0  2023/05/26  11:43:32  forbidden-files/lpt6.txt
      0  2023/05/26  11:43:32  forbidden-files/LPT9
      0  2023/05/26  11:43:32  forbidden-files/NUL
      0  2023/05/26  11:43:32  forbidden-files/com5.txt
      0  2023/05/26  11:43:32  forbidden-files/lpt4.txt
      0  2023/05/26  11:43:32  forbidden-files/COM1
      0  2023/05/26  11:43:32  forbidden-files/COM2
      0  2023/05/26  11:43:32  forbidden-files/con.txt
      0  2023/05/26  11:43:32  forbidden-files/COM3
      0  2023/05/26  11:43:32  forbidden-files/COM4
      0  2023/05/26  11:43:32  forbidden-files/COM5
      0  2023/05/26  11:43:32  forbidden-files/com3.txt
      0  2023/05/26  11:43:32  forbidden-files/lpt2.txt
      0  2023/05/26  11:43:32  forbidden-files/AUX
      0  2023/05/26  11:43:32  forbidden-files/COM6
      0  2023/05/26  11:43:32  forbidden-files/COM7
      0  2023/05/26  11:43:32  forbidden-files/COM8
      0  2023/05/26  11:43:32  forbidden-files/COM9
      0  2023/05/26  11:43:32  forbidden-files/com1.txt
      0  2023/05/26  11:43:32  forbidden-files/PRN
      0  2023/05/26  11:43:32  forbidden-files/aux.txt
      0  2023/05/26  11:43:32  forbidden-files/com9.txt
      0  2023/05/26  11:43:33  forbidden-files/lpt9.txt
      0  2023/05/26  11:43:32  forbidden-files/com8.txt
      0  2023/05/26  11:43:32  forbidden-files/lpt7.txt
         2023/05/26  11:43:33  not-forbidden/
      0  2023/05/26  11:43:33  not-forbidden/auxx.txt
      0  2023/05/26  11:43:33  not-forbidden/prnn.txt
      0  2023/05/26  11:43:33  not-forbidden/CONN
      0  2023/05/26  11:43:33  not-forbidden/NULL
      0  2023/05/26  11:43:33  not-forbidden/null.txt
      0  2023/05/26  11:43:33  not-forbidden/AUXX
      0  2023/05/26  11:43:33  not-forbidden/LPT11
      0  2023/05/26  11:43:33  not-forbidden/lpt11.txt
      0  2023/05/26  11:43:33  not-forbidden/PRNN
      0  2023/05/26  11:43:33  not-forbidden/conn.txt
         2023/05/26  11:43:32  forbidden-suffixes/
      0  2023/05/26  11:43:32  forbidden-suffixes/test. . . .
      0  2023/05/26  11:43:32  forbidden-suffixes/...   
      0  2023/05/26  11:43:32  forbidden-suffixes/R.E.M.
      0  2023/05/26  11:43:32  forbidden-suffixes/   hello   
         2023/05/26  11:43:32  forbidden-chars/
      0  2023/05/26  11:43:32  forbidden-chars/test>test<test.txt
      0  2023/05/26  11:43:32  forbidden-chars/test|test|test.txt
      0  2023/05/26  11:43:32  forbidden-chars/test\test\test.txt
      0  2023/05/26  11:43:32  forbidden-chars/test?test?test.txt
      0  2023/05/26  11:43:32  forbidden-chars/test"test"test.txt
      0  2023/05/26  11:43:32  forbidden-chars/test*test*test.txt
      0  2023/05/26  11:43:32  forbidden-chars/test<test>test.txt
EOF


read status < $status && test "x$status" = xsuccess

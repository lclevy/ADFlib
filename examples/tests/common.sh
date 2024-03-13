set -e

mkdir -vp tmp
tmpdir=`mktemp -d tmp/tmp.XXXXXX`

trap cleanup 0 1 2
cleanup() {
    rm -rf $tmpdir
}

host_type=`uname | sed 's/_.*//'`
echo "Host type: '${host_type}'"
if [ "x${host_type}" = 'xMINGW32' -o \
     "x${host_type}" = 'xMINGW64' ]
then
    expected_dir=$basedir/results_msys
##elif [ "x${host_type}" = 'xCYGWIN' ]; then
else
    expected_dir=$basedir/results
fi
echo "expected_dir: ${expected_dir}"

status=$tmpdir/status
expected=$tmpdir/expected
actual=$tmpdir/actual
echo success >$status
tmpdir4sed=$(echo "$tmpdir" | sed -e 's/\//\\\//g')
basedir4sed=$(echo "$basedir" | sed -e 's/\//\\\//g')


# parameters:
#   - name or description of the test
#   - filename (of the file with expected result/output from the test)
compare_with() {
    #cat >$expected
    sed -e "s/TEMPDIR/$tmpdir4sed/" \
        -e "s/BASEDIR/$basedir4sed/" $expected_dir/$2 > $expected
    if diff -u --strip-trailing-cr $expected $actual; then
	echo "Test '$1' OK."
    else
	echo "Test '$1' failed."
        echo failed >$status
    fi
}

get_test_cmd() {
    cmd_path=${cmd_path:-.} # default directory to search: "."
    if [ ! -d "$cmd_path" ]; then
        echo no-such-command
        echo >&2 cmd_path $cmd_path is not a directory
        exit 1
    fi
    if [ -x "$cmd_path/$1" ]; then
        echo "$cmd_path/$1"
    elif [ -x "$cmd_path/$1.exe" ]; then
        echo "$cmd_path/$1.exe"
    else
        echo no-such-command
        echo >&2 No $1 executable found in `cd "$cmd_path" && pwd`
        exit 1
    fi
}

set -e

tmpdir=`mktemp -d`
trap cleanup 0 1 2
cleanup() {
    rm -rf $tmpdir
}

status=$tmpdir/status
expected=$tmpdir/expected
actual=$tmpdir/actual
echo success >$status
compare_with() {
    cat >$expected
    if diff -u $expected $actual; then
        :
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

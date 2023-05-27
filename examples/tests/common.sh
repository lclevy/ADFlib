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
        echo failed >$status
    fi
}

get_test_cmd() {
    if [ -x ./$1 ]; then
        echo ./$1
    elif [ -x ./$1.exe ]; then
        echo ./$1.exe
    else
        echo no-such-command
        echo >&2 "No $1 executable found in `pwd`"
        exit 1
    fi
}

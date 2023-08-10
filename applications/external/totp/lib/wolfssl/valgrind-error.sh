#!/bin/sh
#
#
# Our valgrind "error" wrapper.

TMP="valgrind.tmp.$$"

valgrind  --suppressions=valgrind-bash.supp --leak-check=full -q "$@" 2> $TMP

result="$?"

# verify no errors

output="`cat $TMP`"

if [ "$output" != "" ]; then
    cat $TMP >&2
    result=1
fi

rm $TMP

exit $result


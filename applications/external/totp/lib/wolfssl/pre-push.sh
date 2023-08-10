#!/bin/bash
#
#
# Our "pre-push" hook.

RESULT=0

if [ -d ./fips ];
then
    echo "\n\nTesting with FIPS release code...\n\n"
    ./fips-check.sh
    RESULT=$?
    [ $RESULT -ne 0 ] && echo -e "\n\nFIPS build test failed" && exit 1
fi

[ $RESULT -ne 0 ] && echo "\nOops, your push failed\n" && exit 1

echo "\nPush tests passed!\n"
exit 0

#!/bin/sh

# Script to cleanup test files
# This is helpful if running ./tests/unit.test as sudo,
# which creates these files with sudoer permissions and
# will cause issues on subsequent tests without sudo

rm -f ./tests/bio_write_test.txt
rm -f ./test-write-dhparams.pem
rm -f ./certeccrsa.pem
rm -f ./certeccrsa.der
rm -f ./ecc-key.der
rm -f ./ecc-key.pem
rm -f ./ecc-public-key.der
rm -f ./tests/test-log-dump-to-file.txt

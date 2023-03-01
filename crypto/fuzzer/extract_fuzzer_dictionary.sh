#!/usr/bin/env bash

# usage: script.sh target-dictionary-filename

# This script searches for interesting strings in the source code and converts
# them into a standard fuzzer dictionary file.
#
# Note that this script is phased out in favor of the more sophisticated
# extract_fuzzer_dictionary.py program

# TODO known issues: the end result has some duplicates in it

TARGET_DIR=../tests
OUTPUT_FILE=${1:-fuzzer_crypto_tests_strings_dictionary1.txt}

multiline_string_search() {
  # TODO the `find` regex behavior is Linux-specific
  find $TARGET_DIR -type f -regextype posix-extended -regex '.*\.(c|h|py|json|java|js)' | xargs cat | perl -p0e 's/"\s*\n\s*\"//smg'
}

# ensure empty file
echo -n "" > $OUTPUT_FILE

# strip multiline strings and extract them
# exclude some hex strings, but allow hex strings with mixed capitalization (Ethereum, rskip60)
multiline_string_search | grep -P -o  "\"[\w ]+\"" | grep -v -P "\"(([0-9a-f][0-9a-f])+|([0-9A-F][0-9A-F])+)\"" | sort | uniq | while read -r line ; do
  echo "$line" >> $OUTPUT_FILE
done

# extract individual BIP39 and SLIP39 words
# TODO are those actually valuable as fuzzer dictionary input?
# grep -r -P -o -h "\"\w+\""  ../slip39_wordlist.h ../bip39_english.h | sort | uniq >> fuzzer_crypto_tests_strings_dictionary1.txt

# extract and convert binary input data from the unit tests
# find each file, cat it, concatenate multiline strings, look for hex strings in quotes
# note that this returns multiple megabyte of result strings due to the large amount
# of test cases in the wycheproof project subfolder
multiline_string_search | grep -P -o "\"([0-9a-fA-F][0-9a-fA-F])+\"" | grep -P -o "([0-9a-fA-F][0-9a-fA-F])+" | sort | uniq | while read -r line ; do
  # turn ascii hex strings AA into \xaa for the fuzzer format and add quotes
  # extra backslash escape due to the bash nesting
  escaped_hex=`echo $line | sed -e 's/../\\\\x&/g'`
  echo "\"$escaped_hex\"" >> $OUTPUT_FILE
done

# search and reassemble BIP39 test seeds that span multiple lines
# find each file, cat it, concatenate multiline strings, look for BIP39 seed combinations with reasonable length
multiline_string_search | grep -Po "(\w{3,10} ){11,23}(\w{3,10})" | sort | uniq | while read -r line ; do
  echo "\"$line\"" >> $OUTPUT_FILE
done

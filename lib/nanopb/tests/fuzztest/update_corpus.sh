#!/bin/bash

# This script is used to update corpus.zip and regressions.zip from the
# Google oss-fuzz project. To actually run this requires access rights to the
# nanopb oss-fuzz storage.
#
# The oss-fuzz project uses separate fuzzer test cases for better performance
# with coverage based fuzzers. This script merges the corpus to a single one
# for the combined fuzztest test case.

set -x
set -e
mkdir tmp-corpusupdate
cd tmp-corpusupdate

gsutil cp gs://nanopb-backup.clusterfuzz-external.appspot.com/corpus/libFuzzer/nanopb_fuzztest_proto2_static/latest.zip proto2_static.zip
gsutil cp gs://nanopb-backup.clusterfuzz-external.appspot.com/corpus/libFuzzer/nanopb_fuzztest_proto2_pointer/latest.zip proto2_pointer.zip
gsutil cp gs://nanopb-backup.clusterfuzz-external.appspot.com/corpus/libFuzzer/nanopb_fuzztest_proto3_static/latest.zip proto3_static.zip
gsutil cp gs://nanopb-backup.clusterfuzz-external.appspot.com/corpus/libFuzzer/nanopb_fuzztest_proto3_pointer/latest.zip proto3_pointer.zip
gsutil cp gs://nanopb-backup.clusterfuzz-external.appspot.com/corpus/libFuzzer/nanopb_fuzztest_io_errors/latest.zip io_errors.zip

unzip -nd corpus ../corpus.zip # Unzip old corpus
unzip -nd new_corpus proto2_static.zip
unzip -nd new_corpus proto2_pointer.zip
unzip -nd new_corpus proto3_static.zip
unzip -nd new_corpus proto3_pointer.zip
unzip -nd new_corpus io_errors.zip

# Build fuzztest with libfuzzer to merge corpuses
CCFLAGS="-DLLVMFUZZER -fsanitize=address -fsanitize-address-use-after-scope -fsanitize=fuzzer-no-link"
LINKFLAGS="-std=c++11 -O1 -fsanitize=address -fsanitize-address-use-after-scope -fsanitize=fuzzer-no-link -stdlib=libc++"
LINKLIBS="-fsanitize=fuzzer"

scons -u CC=clang LINK=clang++ \
    BUILDDIR=fuzztest/tmp-corpusupdate/build build/fuzztest/fuzztest \
    "CCFLAGS=$CCFLAGS" "LINKFLAGS=$LINKFLAGS" "LINKLIBS=$LINKLIBS"

# Copy any files with new features into corpus directory
build/fuzztest/fuzztest corpus new_corpus -merge=1

# Add files to end of the zips. This should work relatively efficiently
# with gits binary diff feature.
(cd corpus; zip -u ../../corpus.zip ./*)
(cd new_corpus/regressions; zip -u ../../regressions.zip *)

cd ..
rm -rf tmp-corpusupdate

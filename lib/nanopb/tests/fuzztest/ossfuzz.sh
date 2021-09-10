#!/bin/bash -eu

# Go to tests folder
cd "$( dirname "${BASH_SOURCE[0]}" )/.."

# Build seed corpus.
# Generating it here ensures it will contain all of the fields in the AllTypes
# test case. The generators are built without fuzzing instrumentation.
rm -rf build
scons build/alltypes/encode_alltypes build/fuzztest/generate_message
mkdir fuzztest_seed_corpus
build/alltypes/encode_alltypes 0 > fuzztest_seed_corpus/alltypes0
build/alltypes/encode_alltypes 1 > fuzztest_seed_corpus/alltypes1
build/alltypes/encode_alltypes 2 > fuzztest_seed_corpus/alltypes2
build/fuzztest/generate_message $(date +%s) > fuzztest_seed_corpus/rndmsg 2>/dev/null
for f in fuzztest_seed_corpus/*; do
    mv $f fuzztest_seed_corpus/$(sha1sum $f | cut -f 1 -d ' ')
done
zip -r "$OUT/corpus.zip" fuzztest_seed_corpus

# Build the fuzz testing stubs with instrumentation
rm -rf build

FUZZERS="build/fuzztest/fuzztest_proto2_static
         build/fuzztest/fuzztest_proto2_pointer
         build/fuzztest/fuzztest_proto3_static
         build/fuzztest/fuzztest_proto3_pointer
         build/fuzztest/fuzztest_io_errors"

scons CC="$CC" CXX="$CXX" LINK="$CXX" \
      CCFLAGS="-Wall -Wextra -g -DLLVMFUZZER $CFLAGS" \
      CXXFLAGS="-Wall -Wextra -g -DLLVMFUZZER $CXXFLAGS" \
      NODEFARGS="1" \
      LINKFLAGS="-std=c++11 $CXXFLAGS" \
      LINKLIBS="$LIB_FUZZING_ENGINE" $FUZZERS

cp $FUZZERS "$OUT"

# The fuzzer test cases are closely related, so use the same seed corpus
# for all of them.
for fuzzer in $FUZZERS
    do cp "$OUT/corpus.zip" "$OUT/$(basename $fuzzer)_seed_corpus.zip"
done
rm "$OUT/corpus.zip"


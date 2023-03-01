# Fuzz Testing trezor-crypto
Selected functions can be fuzzed via specific libFuzzer harnesses for increased test coverage and issue detection.

Note: the following commands are relative to the trezor-crypto main directory.

## Build

A modern C compiler with built-in libFuzzer support is required. The build process will use `clang` by default.
Set the `CC=` environment variable if you want to use a special compiler variant.

```bash
make clean
FUZZER=1 make fuzzer -j$(nproc)
```

### Sanitizers
Recommended: ASAN / UBSAN / MSAN flags for error detection can be specified via the special `SANFLAGS`.

Examples:

* `SANFLAGS="-fsanitize=address,undefined"`
* `SANFLAGS="-fsanitize=memory -fsanitize-memory-track-origins"`

### Optimizations

Override `OPTFLAGS` to test the library at different optimization levels or simplify the debugging of detected issues.

Examples:

* `OPTFLAGS="-O0 -ggdb3"`
* `OPTFLAGS="-O3 -march=native -fno-omit-frame-pointer -gline-tables-only"`

To be determined:

* semi-automatic use of `-fsanitize-ignorelist` to reduce sanitizer overhead on hot functions
* `-flto` and `-flto=thin` link time optimization

Advanced usage:
* [Profile guided optimization](https://clang.llvm.org/docs/UsersManual.html#profile-guided-optimization)

### Fuzzer-specific Configuration Flags

* `-DFUZZ_ALLOW_SLOW` to enable optional fuzzing targets of slow functions
* select a specific fuzz testing harness with `-DFUZZER_EXCLUSIVE_TARGET=` to disable the use of all other targets

### Other Flags

To be determined:

* `-DNDEBUG`
* `-DUSE_BIP39_CACHE=0 -DUSE_BIP32_CACHE=0` to explicitly disable the cache, a workaround for automatic cache clearing is used otherwise
* `-D_FORTIFY_SOURCE=2` together with optimization flag -O2 or above
* `-fstack-protector-strong` or `-fstack-protector-all`
* `-m32` to closer emulate the 32-bit environment present on microcontroller platforms
    * manually adjust Makefile `DSECP256K1_CONTEXT_SIZE=` for 32-bit values, see `legacy/firmware/Makefile`
    * this flag requires 32-bit build support for gcc-multilib, libc and others
    * switching from 64-bit to 32-bit has some effects on sanitizer internals such as Address Sanitizer
* `-DSHA2_UNROLL_TRANSFORM` SHA2 optimization flags
* `-fsanitize-coverage=edge,trace-cmp,trace-div,indirect-calls,trace-gep,no-prune` to add program counter granularity
* starting with clang-15, the additional `trace-loads` and `trace-stores` sanitizer coverage options are also available

## Operation

See the [libFuzzer documentation](https://llvm.org/docs/LibFuzzer.html#options) for valid options and usage. Detailed fuzzer usage and relevant considerations are out of scope of this document.

**Warning**: fuzzing is resource-intensive and can have a negative impact on your system stability.

Basic fuzzer call:
```bash
./fuzzer/fuzzer
```

Here is a more sophisticated multithreading example with a persistent input corpus and other optimizations:
```bash
mkdir fuzzer/fuzzer_corpus
./fuzzer/fuzzer -max_len=2048 -use_value_profile=1 -workers=16 -jobs=16 -timeout=1 -reload=5 -print_pcs=1 -print_funcs=42  fuzzer/fuzzer_corpus
```

Hint: for more permanent setups, consider invoking the fuzzer from outside of the source directory to avoid cluttering it with logfiles and crash inputs. Similarly, it is recommended to store the fuzzer corpus in another location.

## Automated Fuzzer Dictionary Generation

[Dictionaries](https://llvm.org/docs/LibFuzzer.html#dictionaries) are a useful mechanism to augment the capabilities of the fuzzer. Specify them via the `-dict=` flag.

### Collect Interesting Strings From Unit Tests

```bash
cd fuzzer
./extract_fuzzer_dictionary.sh fuzzer_crypto_tests_strings_dictionary1.txt
```
The resulting file can be used as a fuzzer dictionary.

## Evaluate Source Coverage

  1. build the fuzzer binary with `CFLAGS="-fprofile-instr-generate -fcoverage-mapping"`
  1. run with suitable `-runs=` or `-max_total_time=` limits
  1. convert the recorded data `llvm-profdata merge -output=default.profdata -instr default.profraw`
  1. render the data `llvm-cov show fuzzer/fuzzer -instr-profile=default.profdata -format=html -output-dir=coverage-report`
  1. analyze report at `coverage-report/index.html`
  1. (optional) remove artifacts with `rm default.profraw default.profdata && rm -r coverage-report`

## Using Honggfuzz Fuzzer

Although this code is designed primarily for libFuzzer, it can also be used with [Honggfuzz](https://honggfuzz.dev).
However, the usage details are out of scope of this document.

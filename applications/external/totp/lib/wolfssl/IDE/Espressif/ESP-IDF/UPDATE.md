# Espressif Updates

See [#5795]()

## Changes in November 2022

Updates to Espressif ESP-IDF wolfssl_benchmark and wolfssl_test examples:

- Benchmark example no longer runs tests.
- Fixed Test example that was missing code files. (no longer installed locally)
- Rename `benchmark/main` and `test/main` filenames from `helper.c` to `main.c`, cleaned up.
- Added `main.h`
- Revised `main/CMakeLists.txt` to use only `main.c`
- Set components `main` and `wolfssl` for project `CMakeLists.txt`
- Update wolfcrypt esp32_sha.c and sha256.c to now report unexpected reentry as verbose log rather than error message.
- `wolfcrypt/test.c` now returns `args.return_code` when `WOLFSSL_ESPIDF` is defined.
- `wolfcrypt/test.h` now declares `int wolf_test_task(void)` when `WOLFSSL_ESPIDF` is defined.
- `setup.sh` no longer copies benchmark.c(.h) to local project directory.
- Added `libs/Tigard.cfg` file for Tigard JTAG debugger.
- Update `sdkconfig.defaults` with compiler optimizations and stack check.
- Added VisualGDB Project file & Visual Studio solution file.
- Added optional `time_helper` for wolfssl_test
- Exclude `ssl_misc.c` in component cmake to fix warning:  #warning ssl_misc.c does not need to be compiled separately from ssl.c

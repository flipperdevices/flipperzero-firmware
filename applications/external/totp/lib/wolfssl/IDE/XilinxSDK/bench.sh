#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Parametrisation to this script is as follows:
# * The first argument will be taken as suffix to the result folder
#   e.g. `./bench.sh _config0`
# * It is possible to make its output a bit more verbose by setting
#   the `VERBOSE` environment variable to '1', e.g. via
#   `VERBOSE=1 ./bench.sh` or to make it quiet by setting it to '0'
# * Per default we read from /dev/ttyUSB2, by setting the `WC_TTY`
#   environment variable this can adapted
#   e.g. `WC_TTY=/dev/ttyACM0 ./bench.sh`

###
#  Preamble
###

my_path=$(dirname $(readlink -f $0)) || exit $?
readonly my_path
readonly csv_path_suffix="$1"

readonly common_opts="-blocks 128"
readonly ccm_gcm_opts="-all_aad -aad_size 13"

# options used in `small_block()`
readonly cbc_opts="$common_opts"
readonly ccm_opts="$common_opts $ccm_gcm_opts"
readonly gcm_opts="$common_opts $ccm_gcm_opts"
readonly small_block_ciphers="cbc ccm gcm"
readonly small_block_ciphers_hw="gcm"
readonly small_block_sizes="16 528 1024 4112 7696 15888 32768 65536 131072"

# options used in `large_block()`
readonly ccm_fast_opts="$common_opts -all_aad"
readonly gcm_fast_opts="$common_opts -all_aad"
# 512 MiB transfer, 128 MiB max. blocksize
readonly large_block_ciphers="gcm"
readonly large_num_bytes=$((512 * 1024 * 1024))
readonly large_max_blocksize=$((128 * 1024 * 1024))


source "$my_path"/../../scripts/bench/bench_functions.sh


###
#  Implementation
###

[ "$1" == "_HW" ] && small_block "$small_block_ciphers_hw" \
                  || small_block "$small_block_ciphers"

# No large blocksizes for analysis
#[ "$1" == "_HW" ] && large_block

# Benchmark only on HW and SW
if [ "$1" != "_ARMv8" ]; then
    bench "ecc" "-ecc -ecc-all -ecc-kg"

    bench "sha3" "-sha3-384"

    for keysize in 2048 3072 4096
    do
        bench "rsa" "-rsa-sz -rsa-kg $keysize" "_$keysize"
    done

    bench "rng" "-rng"
fi

# Benchmark only on ARMv8 and SW
if [ "$1" != "_HW" ]; then
    bench "sha2" "-sha2"

    bench "cmac" "-cmac"
fi

#eof

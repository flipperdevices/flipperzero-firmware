#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Parametrisation to this script is as follows:
# * `my_path` MUST be set
# * `WC_TTY` can be set to override the default '/dev/ttyUSB2'
# * `csv_path_suffix` can be set to add a suffix to the output path
# * `VERBOSE` can be set to '0' to suppress all output
#                        or '1' to make the output more verbose

###
#  Preamble
###


if (return 0 2>/dev/null); then
    [[ -v my_path ]] || { echo "\$my_path must not be empty"; return 1; }
else
    echo "This script shall only be sourced"
    exit 1
fi

readonly tty="${WC_TTY:-/dev/ttyUSB2}"
fifo="$(mktemp -u)" || exit $?
readonly fifo
readonly csv_path="${my_path}/data/results${csv_path_suffix:-}"

function status_echo() {
    [ "$VERBOSE" = "0" ] || echo "$*"
}

function cleanup() {
    wait
    rm $fifo
}
mkfifo $fifo
trap cleanup EXIT

function error_out() {
    exit 1
}
trap error_out INT TERM

mkdir -p $csv_path

status_echo "Writing to folder: $csv_path"
status_echo "Reading from TTY:  $tty"

###
#  Functions
###

function read_tty() {
    while true; do
        read -r l
        $1 "$l"
        $read_tty_ret
    done < $tty
}

function wait_until_finished() {
    while true; do
        read -r ret
        [ "$ret" == "finished" ] && break
    done < $fifo
}

function process_csv() {
    read_tty_ret=
    case "$csv_state" in
        "0")
            case "$1" in
                "Algorithm,MB/s,Cycles per byte," | \
                "Algorithm,key size,operation,avg ms,ops/sec,")
                    echo "$1" > $csv
                    csv_state=1
                    ;;
            esac
        ;;
        "1")
            if [ "$1" != "Benchmark complete" ]; then
                echo "$1" >> $csv
                [ "$VERBOSE" = "1" ] && echo "$1"
            else
                echo "finished" > $fifo
                read_tty_ret='return'
            fi
            ;;
    esac
}

function csv_start() {
    csv_state=0
    csv=$csv_path/$1
    read_tty process_csv &
}

function bench() {
    status_echo "Benchmark ${1^^}$3"
    csv_start ${1}${3}.csv
    echo "b $2 -csv" > $tty
    wait_until_finished
}

###
#  Implementation
###

function small_block() {
    if [[ ! -v small_block_sizes ]]; then
        echo '$small_block_sizes is unset.' 1>&2
        return 1
    fi
    for blocksize in $small_block_sizes
    do
        status_echo "Benchmark with $blocksize bytes sized blocks"
        for mode in $1
        do
            local opts=${mode}_opts
            bench "${mode}" "-aes-${mode} $blocksize ${!opts}" "_${blocksize}"
        done
    done
}

function large_block() {
    if [[ ! -v large_block_ciphers ]]; then
        echo '$large_block_ciphers is unset.' 1>&2
        return 1
    fi
    if [[ ! -v large_max_blocksize ]]; then
        echo '$large_max_blocksize is unset.' 1>&2
        return 1
    fi
    if [[ ! -v large_num_bytes ]]; then
        echo '$large_num_bytes is unset.' 1>&2
        return 1
    fi
    # 1 MiB
    local blocksize=$((1024 * 1024))
    while [ $blocksize -lt $large_max_blocksize ]
    do
        local num_blocks=$(($large_num_bytes / $blocksize))
        status_echo "Benchmark with $blocksize bytes sized blocks"
        for mode in $large_block_ciphers
        do
            local opts=${mode}_fast_opts
            bench "${mode}" "-aes-${mode} ${!opts} $blocksize -blocks $num_blocks" "_${blocksize}"
        done
        blocksize=$(($blocksize * 2))
    done
}

#eof

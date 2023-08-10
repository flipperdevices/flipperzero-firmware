#!/usr/bin/env bash
#
# SPDX-License-Identifier: GPL-2.0-or-later
#
# Parametrisation to this script is as follows:
# * none yet

###
#  Preamble
###

my_path="$(dirname $(readlink -f $0))" || exit $?
readonly my_path
readonly csv_path="$my_path/data"

function cleanup() {
    echo OK
}
trap cleanup EXIT

function error_out() {
    exit 1
}
trap error_out INT TERM

###
#  Functions
###



###
#  Implementation
###

configs=$(find $csv_path -maxdepth 1 -type d -name '*results_*' | sed 's@.*results_@@g') || exit $?
readonly configs

declare -A algos
algos["asym"]="ecc rsa"
algos["hashes"]="sha2 sha3"
algos["mac"]="cmac"
algos["rng"]="rng"
algos["sym"]="cbc ccm gcm"

declare -A headers
headers["asym"]="config,keysize_2,algorithm,keysize,operation,avg ms,ops/sec,"
headers["hashes"]="config,unused,algorithm,MiB/s,"
headers["mac"]="config,unused,algorithm,keysize,MiB/s,"
headers["rng"]="config,unused,algorithm,MiB/s,"
headers["sym"]="config,chunksize,algorithm,blocksize,direction,AAD,MiB/s,"

# "... now you have two problems"
declare -A filters
filters["ccm"]="-e s/-\(enc\|dec\)-\(no_AAD\|custom\)/,128,\1,\2/g -e s/-\(enc\|dec\),/,128,\1,,/g"
filters["gcm"]="-e /-192-/d -e /GMAC/d -e s/-\(enc\|dec\)-\(no_AAD\|custom\)/,\1,\2/g -e s/-\(enc\|dec\),/,\1,default,/g -e s/\(128\|256\)-GCM/GCM,\1/g"
filters["cbc"]="-e /-192-/d -e s/-\(enc\|dec\),/,\1,,/g -e s/\(128\|256\)-CBC/CBC,\1/g"
filters["cmac"]="-e s/\(128\|256\)-CMAC/CMAC,\1/g"
filters["ecc"]='-e 1!{/SECP384R1\|SECP521R1/!d}'
filters["sha2"]="-e s/SHA-/SHA2-/g"

for t in "${!algos[@]}"
do
    for algo in ${algos[$t]}
    do
        outfile="$csv_path/combined_${algo}.csv"
        echo ${headers[$t]} > "$outfile"
        for cfg in $configs
        do
            for f in $(find $csv_path/results_${cfg} -name "*${algo}*.csv" | sort -V)
            do
                sz=$(basename $f | sed -e s/${algo}// -e s/_// -e s/\.csv//)
                sz=",$sz"
                for l in $(tail -n +2 $f | tr -d ' ')
                do
                    echo "${cfg}${sz},${l}" >> "$outfile"
                done
                [ "${filters[$algo]}" == "" ] || sed -i "$outfile" ${filters[$algo]}
            done
            echo $algo $t $cfg
        done
    done
done

#!/usr/bin/env bash
# shellcheck disable=SC2043 # noise.  fine for a loop to run only once.
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
readonly log_path="$csv_path/logs"
readonly img_path="$csv_path/images"
readonly gnuplot_terminal="svg enhanced background rgb 'white' size 800 600"
mkdir -p "$log_path"
mkdir -p "$img_path"

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


function cg() {
    csvgrep -c $1 -r $2 $3
}


###
#  Implementation
###

readonly desc_block_ciphers="Benchmarks were done with growing sample size and averaging over\nprocessing of 128 samples of said sample size"
readonly desc_asymmetric="Benchmarks were done with averaging over\nas many repetitions possible of the benchmarked operation in 1s"
readonly desc_others="Benchmarks were done with 1MiB block size and averaging over\nas many repetitions possible of processing 5MiB data in 1s"

configs=$(find $csv_path -type d -name '*results*' | sed 's@.*results_@@g') || exit $?
readonly configs

###
#  Symmetric crypto

readonly sym="cbc ccm gcm"

declare -A directions
directions["dec"]="decryption"
directions["enc"]="encryption"

declare -A aad_sizes
aad_sizes["no_AAD"]="0 Bytes"
aad_sizes["custom"]="13 Bytes"
aad_sizes["default"]="16 Bytes"

# not pretty but works for me :)
# CBC&GCM encryption is in software a lot faster than decryption,
# therefore use the same Range on the Y-Axis to also have a visual indication.
# This will break if something changes, so let the user override the value
cbc_yrange="${cbc_yrange:=1400}"
gcm_yrange="${gcm_yrange:=500}"

for mode in $sym
do
    infile="$csv_path/combined_${mode}.csv"
    for dir in "${!directions[@]}"
    do
        plotstring=
        more_style=
        for cfg in $configs
        do
            for bsize in $(csvcut -c blocksize $infile | tail -n +2 | sort -u)
            do
                if [ "$mode" == "cbc" ]; then
                    outfile=$log_path/${mode}${bsize}_${cfg}_${dir}.log
                    val="$(cg config $cfg $infile | cg blocksize $bsize | cg direction $dir | csvcut -c chunksize,MiB/s | tail -n +2 | tr ',' ' ')"
                    if [ "$val" != "" ]; then
                        echo "$val" > $outfile
                        [ -z "$plotstring" ] && plotstring="plot" || plotstring="${plotstring},"
                        plotstring="${plotstring} '$outfile' smooth bezier title \"$cfg AES$bsize\""
                    fi
                    [ "$mode" == "cbc" -a "$cbc_yrange" != "" ] && more_style="set yrange [ 0 : $cbc_yrange ]"
                else
                    for aad in "${!aad_sizes[@]}"
                    do
                        outfile=$log_path/${mode}${bsize}_${cfg}_${dir}_${aad}.log
                        val="$(cg config $cfg $infile | cg blocksize $bsize | cg direction $dir | cg AAD $aad | csvcut -c chunksize,MiB/s | tail -n +2 | tr ',' ' ')"
                        if [ "$val" != "" ]; then
                            echo "$val" > $outfile
                            [ -z "$plotstring" ] && plotstring="plot" || plotstring="${plotstring},"
                            plotstring="${plotstring} '$outfile' smooth bezier title \"$cfg AES$bsize ${aad_sizes[$aad]} AAD\""
                        fi
                    done
                    [ "$mode" == "gcm" -a "$gcm_yrange" != "" ] && more_style="set yrange [ 0 : $gcm_yrange ]"
                fi
            done
        done
        modep=${mode^^}
        gnuplot << EOF
set terminal $gnuplot_terminal
set title "${modep} ${directions[${dir}]} on Xilinx Versal"
set x2label "${desc_block_ciphers}"
set xlabel "Sample size"
set ylabel "MiB/s"
set style data lines
$more_style

set output "${img_path}/${mode}_${dir}.${gnuplot_terminal%% *}"
$plotstring
EOF
    done
done

###
#  Also create separate graphs for AES-GCM-128 and -256 since that's too many lines in one image

for mode in gcm
do
    infile="$csv_path/combined_${mode}.csv"
    for dir in "${!directions[@]}"
    do
        for bsize in $(csvcut -c blocksize $infile | tail -n +2 | sort -u)
        do
            plotstring=
            more_style="set yrange [ 0 : $gcm_yrange ]"
            for cfg in $configs
            do
                for aad in "${!aad_sizes[@]}"
                do
                    outfile=$log_path/${mode}${bsize}_${cfg}_${dir}_${aad}.log
                    val="$(cg config $cfg $infile | cg blocksize $bsize | cg direction $dir | cg AAD $aad | csvcut -c chunksize,MiB/s | tail -n +2 | tr ',' ' ')"
                    if [ "$val" != "" ]; then
                        echo "$val" > $outfile
                        [ -z "$plotstring" ] && plotstring="plot" || plotstring="${plotstring},"
                        plotstring="${plotstring} '$outfile' smooth bezier title \"$cfg AES$bsize ${aad_sizes[$aad]} AAD\""
                    fi
                done
            done
            modep=${mode^^}
            gnuplot << EOF
set terminal $gnuplot_terminal
set title "${modep}-${bsize} ${directions[${dir}]} on Xilinx Versal"
set x2label "${desc_block_ciphers}"
set xlabel "Sample size"
set ylabel "MiB/s"
set style data lines
$more_style

set output "${img_path}/${mode}${bsize}_${dir}.${gnuplot_terminal%% *}"
$plotstring
EOF
        done
    done
done

###
#  Asymmetric crypto

declare -A asym_operations
asym_operations["ecc"]="keygen agree sign verify"
asym_operations["rsa"]="keygen public private"

for algo in "${!asym_operations[@]}"
do
    infile="$csv_path/combined_${algo}.csv"
    for op in ${asym_operations[$algo]}
    do
        outfile=$log_path/${algo}_${op}.log
        echo -n "" > $outfile
        for ksize in $(csvcut -c keysize $infile | tail -n +2 | sort -u)
        do
            for cfg in $configs
            do
                h="${algo^^}-$ksize"
                v=$(cg config $cfg $infile | cg operation $op | cg keysize $ksize | csvcut -c "avg ms" | tail -n +2 | tr ',' ' ')
                [ "$v" != "" ] && echo "$h\n$cfg $v" >> $outfile
            done
        done
        algop=${algo^^}
        gnuplot << EOF
set terminal $gnuplot_terminal
set title "${algop} ${op} on Xilinx Versal"
set x2label "${desc_asymmetric}"
set xlabel 'Configurations' offset 0,-1
set ylabel "ms per op"
set style fill solid
set style line 1 lc rgb "grey50"
set boxwidth 0.5
unset key


set output "${img_path}/${algo}_${op}.${gnuplot_terminal%% *}"
plot "$outfile" using :2:xtic(1) with boxes ls 1, \
             '' using :2:2 w labels offset 0,0.7
EOF
    done
done

###
#  Hashes

declare -A hash_sizes
hash_sizes["sha2"]="224 256 384 512"
hash_sizes["sha3"]="384"

plotstring=
outfile=$log_path/sha.log
echo -n "" > $outfile
for algo in "${!hash_sizes[@]}"
do
    infile="$csv_path/combined_${algo}.csv"
    for hsize in ${hash_sizes[$algo]}
    do
        for cfg in $configs
        do
            h="${algo^^}-$hsize"
            v=$(cg config $cfg $infile | cg algorithm $h | csvcut -c MiB/s | tail -n +2 | sed 's@\..*$@@')
            [ "$v" != "" ] && echo "$h\n$cfg $v" >> $outfile
        done
        echo >> $outfile
    done
done

gnuplot << EOF
set terminal $gnuplot_terminal
set title "SHA2/SHA3 on Xilinx Versal"
set x2label "${desc_others}"
set xlabel 'Configurations' offset 0,-1
set ylabel "MiB/s"
set style fill solid
set style line 1 lc rgb "grey50"
set boxwidth 0.5
unset key


set output "${img_path}/sha.${gnuplot_terminal%% *}"
plot "$outfile" using :2:xtic(1) with boxes ls 1, \
             '' using :2:2 w labels offset 0,0.7
EOF

###
#  MACs

declare -A macs
macs["cmac"]="128 256"

plotstring=
outfile=$log_path/mac.log
echo -n "" > $outfile
for algo in "${!macs[@]}"
do
    infile="$csv_path/combined_${algo}.csv"
    for hsize in ${macs[$algo]}
    do
        for cfg in $configs
        do
            h="${algo^^}-$hsize"
            v=$(cg config $cfg $infile | cg keysize $hsize | csvcut -c MiB/s | tail -n +2 | sed 's@\..*$@@')
            [ "$v" != "" ] && echo "$h\n$cfg $v" >> $outfile
        done
        echo >> $outfile
    done
done

gnuplot << EOF
set terminal $gnuplot_terminal
set title "MAC's on Xilinx Versal"
set x2label "${desc_others}"
set xlabel 'Configurations' offset 0,-1
set ylabel "MiB/s"
set style fill solid
set style line 1 lc rgb "grey50"
set boxwidth 0.5
unset key
set yrange [ 0 : 500 ]


set output "${img_path}/mac.${gnuplot_terminal%% *}"
plot "$outfile" using :2:xtic(1) with boxes ls 1, \
             '' using :2:2 w labels offset 0,0.7
EOF

###
#  RNG

plotstring=
outfile=$log_path/rng.log
echo -n "" > $outfile
for algo in rng
do
    infile="$csv_path/combined_${algo}.csv"
    for cfg in $configs
    do
        h="${algo^^}"
        v=$(cg config $cfg $infile | csvcut -c MiB/s | tail -n +2 | sed 's@\..*$@@')
        [ "$v" != "" ] && echo "$h\n$cfg $v" >> $outfile
    done
    echo >> $outfile
done

gnuplot << EOF
set terminal $gnuplot_terminal
set title "RNG on Xilinx Versal"
set x2label "${desc_others}"
set xlabel 'Configurations' offset 0,-1
set ylabel "MiB/s"
set style fill solid
set style line 1 lc rgb "grey50"
set boxwidth 0.5
unset key
set yrange [ 0 : 50 ]

with_unit(Value,Unit) = sprintf("%d %s", Value, Unit)
set output "${img_path}/rng.${gnuplot_terminal%% *}"
plot "$outfile" using :2:xtic(1) with boxes ls 1, \
             '' using :2:2 w labels offset 0,0.7
EOF

#eof

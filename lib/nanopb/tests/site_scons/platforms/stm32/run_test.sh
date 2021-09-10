#!/bin/bash

BINARY=$1
BASENAME=$(basename $1)
shift
ARGS=$*

test X$OPENOCD_BOARD == X && export OPENOCD_BOARD=board/stm32f7discovery.cfg

timeout 1200s openocd -f $OPENOCD_BOARD \
    -c "reset_config srst_only srst_nogate connect_assert_srst" \
    -c "init" -c "arm semihosting enable" \
    -c "arm semihosting_cmdline $BASENAME $ARGS" \
    -c "reset halt" \
    -c "load_image $BINARY 0" \
    -c "reset halt" -c "resume 0x20000040" 2>openocd.log

RESULT=$?

if [ "$RESULT" -ne "0" ]
then
    cat openocd.log >&2
    echo >&2
fi

exit $RESULT



#pragma once

#include <infrared_transmit.h>
#include <infrared_worker.h>

#include "furi_hal.h"

#define AC_ON 1
#define AC_OFF 2

#define AC_MODE_HEAT 3
#define AC_MODE_COLD 4
#define AC_MODE_DRY 5
#define AC_MODE_AUTO 6

#define AC_FAN_SPEED_1 7
#define AC_FAN_SPEED_2 8
#define AC_FAN_SPEED_3 9
#define AC_FAN_SPEED_4 10
#define AC_FAN_SPEED_5 11
#define AC_FAN_SPEED_AUTO 12
#define AC_FAN_SPEED_SILENT 13

#define AC_VANE_AUTO 14
#define AC_VANE_H1 15
#define AC_VANE_H2 16
#define AC_VANE_H3 17
#define AC_VANE_H4 18
#define AC_VANE_H5 19
#define AC_VANE_AUTO_MOVE 20

#define AC_TOO_COOL 21
#define AC_TOO_WARM 22

#define AC_MIN_TEMPERATURE 16
#define AC_MAX_TEMPERATURE 31

#define AC_MITSUBISHI_HDR_MARK 3400
#define AC_MITSUBISHI_HDR_SPACE 1750
#define AC_MITSUBISHI_BIT_MARK 450
#define AC_MITSUBISHI_ONE_SPACE 1300
#define AC_MISTUBISHI_ZERO_SPACE 420
#define AC_MITSUBISHI_RPT_MARK 440
#define AC_MITSUBISHI_RPT_SPACE 17100

typedef struct {
    int ac_power;
    int ac_mode;
    int ac_temp;
    int ac_fanmode;
    int ac_vanemode;
} ACState;

void send_ac_mitsubishi(ACState* acstate);

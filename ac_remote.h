#pragma once

#include <infrared_transmit.h>
#include <infrared_worker.h>

#include "furi_hal.h"

#define AC_HOT 0
#define AC_COLD 1
#define AC_DRY 2
#define AC_AUTO 3

#define FAN_SPEED_1 0
#define FAN_SPEED_2 1
#define FAN_SPEED_3 2
#define FAN_SPEED_4 3
#define FAN_SPEED_5 4
#define FAN_SPEED_AUTO 5
#define FAN_SPEED_SILENT 6

#define VANNE_AUTO 0
#define VANNE_H1 1
#define VANNE_H2 2
#define VANNE_H3 3
#define VANNE_H4 4
#define VANNE_H5 5
#define VANNE_AUTO_MOVE 6

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
    int ac_vannemode;
} ACState;

void send_ac_mitsubishi(ACState* acstate);

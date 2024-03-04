#pragma once

#include <furi_hal.h>

void  set_dcf77_time(FuriHalRtcDateTime *dt, bool is_dst);
int   get_dcf77_bit(int sec);
char *get_dcf77_data(int sec);
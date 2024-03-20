#pragma once

#include <datetime/datetime.h>
#include <furi.h>

void set_dcf77_time(DateTime* dt, bool is_dst);
bool get_dcf77_bit(int sec);
char* get_dcf77_data(int sec);
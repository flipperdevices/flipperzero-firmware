#pragma once

#include <stdbool.h>

/* Initialize Driver */
void bq25896_init();

/* Send device into shipping mode */
void bq25896_poweroff();

/* Is currently charging */
bool bq25896_is_charging();
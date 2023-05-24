#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../desktop.h"
#include <desktop/desktop_settings.h>

#ifdef __cplusplus
extern "C" {
#endif

void desktop_pin_lock_error_notify();

uint32_t desktop_pin_lock_get_fail_timeout();

bool desktop_pin_compare(const PinCode* pin_code1, const PinCode* pin_code2);

#ifdef __cplusplus
}
#endif

#pragma once
#include <stdbool.h>
#include <stdint.h>
#include "../desktop.h"
#include <desktop/desktop_settings.h>

void desktop_pin_lock_error_notify();

uint32_t desktop_pin_lock_get_fail_timeout();

bool desktop_pin_lock_verify(const PinCode* pin_set, const PinCode* pin_entered);

bool desktop_pins_are_equal(const PinCode* pin_code1, const PinCode* pin_code2);

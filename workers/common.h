#pragma once
#include <stdint.h>
#include "../types/token_info.h"

typedef void (*TOTP_AUTOMATION_PRESS_KEY)(uint8_t key);

void totp_type_code_worker_execute_automation(
    TOTP_AUTOMATION_PRESS_KEY key_press_fn,
    const char* string,
    uint8_t string_length,
    TokenAutomationFeature features);
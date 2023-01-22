#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include <furi_hal_gpio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* helper for easier signal generation */
#define ADVANCED_PWM_MS(x) (x * 100000000UL)
#define ADVANCED_PWM_US(x) (x * 100000UL)
#define ADVANCED_PWM_NS(x) (x * 100UL)
#define ADVANCED_PWM_PS(x) (x / 10UL)

/* using an anonymous type for the internals */
typedef struct AdvancedPwmInternals AdvancedPwmInternals;

/* and a public one for accessing user-side fields */
typedef struct AdvancedPwm {
    bool polarity;
    uint32_t edge_timings[2];
    uint32_t reload_reg_buff[2];
    AdvancedPwmInternals* internals;
} AdvancedPwm;

AdvancedPwm* advanced_pwm_alloc();

void advanced_pwm_set_frequency(AdvancedPwm* signal, uint32_t freq, uint32_t duty);

void advanced_pwm_set_timings(AdvancedPwm* signal, uint32_t high, uint32_t low);

void advanced_pwm_reset_gpio(AdvancedPwm* signal);

void advanced_pwm_add_gpio(AdvancedPwm* signal, const GpioPin* gpio);

void advanced_pwm_del_gpio(AdvancedPwm* signal, const GpioPin* gpio);

void advanced_pwm_set_polarity(AdvancedPwm* signal, bool polarity);

void advanced_pwm_free(AdvancedPwm* signal);

void advanced_pwm_prepare_arr(AdvancedPwm* signal);

bool advanced_pwm_get_polarity(AdvancedPwm* signal);

void advanced_pwm_start(AdvancedPwm* signal);

void advanced_pwm_stop(AdvancedPwm* signal);

#ifdef __cplusplus
}
#endif

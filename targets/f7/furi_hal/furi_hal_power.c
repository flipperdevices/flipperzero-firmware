#include <furi_hal_power.h>
// Keep necessary includes for types used in function signatures
#include <furi.h> // For FURI_NORETURN, basic types, PropertyValueCallback
#include <stdbool.h>
#include <stdint.h>


 #include <furi_hal_clock.h>
 #include <furi_hal_bt.h>
 #include <furi_hal_vibro.h>
 #include <furi_hal_resources.h>
 #include <furi_hal_serial_control.h>
 #include <furi_hal_rtc.h>
 #include <furi_hal_debug.h>
 #include <stm32wbxx_ll_rcc.h>
 #include <stm32wbxx_ll_pwr.h>
 #include <stm32wbxx_ll_hsem.h>
 #include <stm32wbxx_ll_cortex.h>
 #include <stm32wbxx_ll_gpio.h>
 #include <hsem_map.h>
 #include <bq27220.h>
 #include <bq27220_data_memory.h>
 #include <bq25896.h>

// Remove TAG definition as logging won't happen
// #define TAG "FuriHalPower"

// Remove debug GPIO defines
// #ifndef FURI_HAL_POWER_DEBUG_WFI_GPIO
// #define FURI_HAL_POWER_DEBUG_WFI_GPIO (&gpio_ext_pb2)
// #endif
// #ifndef FURI_HAL_POWER_DEBUG_STOP_GPIO
// #define FURI_HAL_POWER_DEBUG_STOP_GPIO (&gpio_ext_pc3)
// #endif

// Remove STOP_MODE define
// #ifndef FURI_HAL_POWER_STOP_MODE
// #define FURI_HAL_POWER_STOP_MODE (LL_PWR_MODE_STOP2)
// #endif

// Remove internal state struct
// typedef struct {
//     volatile uint8_t insomnia;
//     volatile uint8_t suppress_charge;
//     bool gauge_ok;
//     bool charger_ok;
// } FuriHalPower;

// Remove global state variable
// static volatile FuriHalPower furi_hal_power = {
//     .insomnia = 0,
//     .suppress_charge = 0,
//     .gauge_ok = false,
//     .charger_ok = false,
// };

// Remove extern declaration
// extern const BQ27220DMData furi_hal_power_gauge_data_memory[];

void furi_hal_power_init(void) {
    // Do nothing
}

bool furi_hal_power_gauge_is_ok(void) {
    // Return a default "OK" state
    return true;
}

bool furi_hal_power_is_shutdown_requested(void) {
    // Return a default "not requested" state
    return false;
}

uint16_t furi_hal_power_insomnia_level(void) {
    // Return a default "no insomnia" state
    return 0;
}

void furi_hal_power_insomnia_enter(void) {
    // Do nothing
}

void furi_hal_power_insomnia_exit(void) {
    // Do nothing
}

bool furi_hal_power_sleep_available(void) {
    // Return a default "always available" state
    return true;
}

// Remove internal static functions as they are no longer needed
// static inline bool furi_hal_power_deep_sleep_available(void) { ... }
// static inline void furi_hal_power_light_sleep(void) { ... }
// static inline void furi_hal_power_suspend_aux_periphs(void) { ... }
// static inline void furi_hal_power_resume_aux_periphs(void) { ... }
// static inline void furi_hal_power_deep_sleep(void) { ... }

void furi_hal_power_sleep(void) {
    // Do nothing (don't actually sleep)
}

uint8_t furi_hal_power_get_pct(void) {
    // Return a default battery percentage
    return 50;
}

uint8_t furi_hal_power_get_bat_health_pct(void) {
    // Return a default battery health percentage
    return 100;
}

bool furi_hal_power_is_charging(void) {
    // Return a default "not charging" state
    return false;
}

bool furi_hal_power_is_charging_done(void) {
    // Return a default "not charged" state (consistent with not charging)
    return false;
}

FURI_NORETURN void furi_hal_power_shutdown(void) {
    // Must not return
    while(1) {
    }
}

void furi_hal_power_off(void) {
    // Do nothing
}

FURI_NORETURN void furi_hal_power_reset(void) {
    // Must not return
    while(1) {
    }
}

bool furi_hal_power_enable_otg(void) {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_power);
    bq25896_set_boost_lim(&furi_hal_i2c_handle_power, BoostLim_2150);
    bq25896_enable_otg(&furi_hal_i2c_handle_power);
    furi_delay_ms(30);
    bool ret = bq25896_is_otg_enabled(&furi_hal_i2c_handle_power);
    bq25896_set_boost_lim(&furi_hal_i2c_handle_power, BoostLim_1400);
    furi_hal_i2c_release(&furi_hal_i2c_handle_power);
    return ret;
}

void furi_hal_power_disable_otg(void) {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_power);
    bq25896_disable_otg(&furi_hal_i2c_handle_power);
    furi_hal_i2c_release(&furi_hal_i2c_handle_power);
}

bool furi_hal_power_is_otg_enabled(void) {
    furi_hal_i2c_acquire(&furi_hal_i2c_handle_power);
    bool ret = bq25896_is_otg_enabled(&furi_hal_i2c_handle_power);
    furi_hal_i2c_release(&furi_hal_i2c_handle_power);
    return ret;
}

float furi_hal_power_get_battery_charge_voltage_limit(void) {
    // Return a typical default limit
    return 4.2f;
}

void furi_hal_power_set_battery_charge_voltage_limit(float voltage) {
    // Do nothing
    (void)voltage; // Suppress unused parameter warning
}

bool furi_hal_power_check_otg_fault(void) {
    // Return a default "no fault" state
    return false;
}

void furi_hal_power_check_otg_status(void) {
    // Do nothing
}

uint32_t furi_hal_power_get_battery_remaining_capacity(void) {
    // Return a default capacity (e.g., in mAh)
    return 1000;
}

uint32_t furi_hal_power_get_battery_full_capacity(void) {
    // Return a default capacity (e.g., in mAh)
    return 2000;
}

uint32_t furi_hal_power_get_battery_design_capacity(void) {
    // Return a default capacity (e.g., in mAh)
    return 2100;
}

float furi_hal_power_get_battery_voltage(FuriHalPowerIC ic) {
    // Return a typical nominal voltage
    (void)ic; // Suppress unused parameter warning
    return 3.7f;
}

float furi_hal_power_get_battery_current(FuriHalPowerIC ic) {
    // Return a default zero current
    (void)ic; // Suppress unused parameter warning
    return 0.0f;
}

// Remove internal static function
// static float furi_hal_power_get_battery_temperature_internal(FuriHalPowerIC ic) { ... }

float furi_hal_power_get_battery_temperature(FuriHalPowerIC ic) {
    // Return a default room temperature (in Celsius)
    (void)ic; // Suppress unused parameter warning
    return 25.0f;
}

float furi_hal_power_get_usb_voltage(void) {
    // Return a default voltage (0.0f assumes disconnected)
    return 0.0f;
}

void furi_hal_power_enable_external_3_3v(void) {
    // Do nothing
}

void furi_hal_power_disable_external_3_3v(void) {
    // Do nothing
}

void furi_hal_power_suppress_charge_enter(void) {
    // Do nothing
}

void furi_hal_power_suppress_charge_exit(void) {
    // Do nothing
}

void furi_hal_power_info_get(PropertyValueCallback out, char sep, void* context) {
    // Do nothing, don't call the callback
    (void)out;
    (void)sep;
    (void)context;
}

void furi_hal_power_debug_get(PropertyValueCallback out, void* context) {
    // Do nothing, don't call the callback
    (void)out;
    (void)context;
}
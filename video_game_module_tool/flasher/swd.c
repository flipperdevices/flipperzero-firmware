#include "swd.h"

#include <furi.h>
#include <furi_hal_resources.h>

#define TAG "VgmSwd"

#define SWD_REQUEST_LEN (8U)
#define SWD_RESPONSE_LEN (3U)
#define SWD_DATA_LEN (32U)

#define SWD_ALERT_SEQUENCE_0 (0x6209F392UL)
#define SWD_ALERT_SEQUENCE_1 (0x86852D95UL)
#define SWD_ALERT_SEQUENCE_2 (0xE3DDAFE9UL)
#define SWD_ALERT_SEQUENCE_3 (0x19BC0EA2UL)

#define SWD_ACTIVATION_CODE (0x1AU)

#define SWD_SLEEP_SEQUENCE (0xE3BCU)

#define SWD_READ_REQUEST_INIT (0x85U)
#define SWD_WRITE_REQUEST_INIT (0x81U)
#define SWD_REQUEST_INIT (0x81U)

typedef enum {
    SwdioDirectionIn,
    SwdioDirectionOut,
} SwdioDirection;

typedef enum {
    SwdResponseOk = 1U,
    SwdResponseWait = 2U,
    SwdResponseFault = 4U,
    SwdResponseNone = 7U,
} SwdResponse;

typedef enum {
    SwdAccessTypeDp = 0U << 1,
    SwdAccessTypeAp = 1U << 1,
} SwdAccessType;

typedef enum {
    SwdAccessDirectionWrite = 0U << 2,
    SwdAccessDirectionRead = 1U << 2,
} SwdAccessDirection;

#ifdef SWD_ENABLE_CYCLE_DELAY
// Slows SWCLK down, useful for debugging via logic analyzer
__attribute__((always_inline)) static inline void swd_delay_half_cycle(void) {
    asm volatile("nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n"
                 "nop \n");
}
#else
#define swd_delay_half_cycle()
#endif

static void __attribute__((optimize("-O3"))) swd_turnaround(SwdioDirection mode) {
    static SwdioDirection prev_dir = SwdioDirectionIn;

    if(prev_dir == mode) {
        return;
    } else {
        prev_dir = mode;
    }

    if(mode == SwdioDirectionIn) {
        // Using LL functions for performance reasons
        LL_GPIO_SetPinMode(gpio_swdio.port, gpio_swdio.pin, LL_GPIO_MODE_INPUT);
    } else {
        furi_hal_gpio_write(&gpio_swclk, false);
    }
    swd_delay_half_cycle();

    furi_hal_gpio_write(&gpio_swclk, true);
    swd_delay_half_cycle();

    if(mode == SwdioDirectionOut) {
        furi_hal_gpio_write(&gpio_swclk, false);
        // Using LL functions for performance reasons
        LL_GPIO_SetPinMode(gpio_swdio.port, gpio_swdio.pin, LL_GPIO_MODE_OUTPUT);
    }
}

static void __attribute__((optimize("-O3"))) swd_tx(uint32_t data, uint32_t n_cycles) {
    swd_turnaround(SwdioDirectionOut);

    for(uint32_t i = 0; i < n_cycles; ++i) {
        furi_hal_gpio_write(&gpio_swclk, false);
        furi_hal_gpio_write(&gpio_swdio, data & (1UL << i));
        swd_delay_half_cycle();

        furi_hal_gpio_write(&gpio_swclk, true);
        swd_delay_half_cycle();
    }

    furi_hal_gpio_write(&gpio_swclk, false);
}

static void __attribute__((optimize("-O3"))) swd_tx_parity(uint32_t data, uint32_t n_cycles) {
    const int parity = __builtin_parity(data);
    swd_tx(data, n_cycles);
    furi_hal_gpio_write(&gpio_swdio, parity);
    swd_delay_half_cycle();
    furi_hal_gpio_write(&gpio_swclk, true);
    swd_delay_half_cycle();
    furi_hal_gpio_write(&gpio_swclk, false);
}

static uint32_t __attribute__((optimize("-O3"))) swd_rx(uint32_t n_cycles) {
    uint32_t ret = 0;
    swd_turnaround(SwdioDirectionIn);

    for(uint32_t i = 0; i < n_cycles; ++i) {
        furi_hal_gpio_write(&gpio_swclk, false);
        ret |= furi_hal_gpio_read(&gpio_swdio) ? (1UL << i) : 0;
        swd_delay_half_cycle();

        furi_hal_gpio_write(&gpio_swclk, true);
        swd_delay_half_cycle();
    }

    furi_hal_gpio_write(&gpio_swclk, false);
    return ret;
}

static bool __attribute__((optimize("-O3"))) swd_rx_parity(uint32_t* data, uint32_t n_cycles) {
    const uint32_t rx_value = swd_rx(n_cycles);
    swd_delay_half_cycle();

    const bool parity_calc = __builtin_parity(rx_value);
    const bool parity_rx = furi_hal_gpio_read(&gpio_swdio);

    furi_hal_gpio_write(&gpio_swclk, true);
    swd_delay_half_cycle();
    furi_hal_gpio_write(&gpio_swclk, false);

    if(data) {
        *data = rx_value;
    }

    return parity_calc == parity_rx;
}

static void swd_line_reset(bool idle_cycles) {
    swd_tx(0xFFFFFFFFUL, 32U);
    swd_tx(0x0FFFFFFFUL, idle_cycles ? 32U : 24U);
}

static void swd_leave_dormant_state(void) {
    swd_line_reset(false);
    swd_tx(SWD_ALERT_SEQUENCE_0, 32U);
    swd_tx(SWD_ALERT_SEQUENCE_1, 32U);
    swd_tx(SWD_ALERT_SEQUENCE_2, 32U);
    swd_tx(SWD_ALERT_SEQUENCE_3, 32U);
    swd_tx(SWD_ACTIVATION_CODE << 4U, 12U);
}

static void swd_enter_dormant_state(void) {
    swd_line_reset(false);
    swd_tx(SWD_SLEEP_SEQUENCE, 16U);
}

void swd_init(void) {
    furi_hal_gpio_init_ex(
        &gpio_swclk, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh, GpioAltFnUnused);
    furi_hal_gpio_init_ex(
        &gpio_swdio, GpioModeOutputPushPull, GpioPullNo, GpioSpeedVeryHigh, GpioAltFnUnused);

    swd_leave_dormant_state();
    swd_line_reset(true);
}

void swd_deinit(void) {
    swd_enter_dormant_state();
    furi_hal_gpio_init_simple(&gpio_swclk, GpioModeAnalog);
    furi_hal_gpio_init_simple(&gpio_swdio, GpioModeAnalog);
}

static inline uint8_t swd_prepare_request(
    SwdAccessDirection access_direction,
    SwdAccessType access_type,
    uint8_t address) {
    uint8_t ret = SWD_REQUEST_INIT | access_type | access_direction | (address << 3);
    ret |= __builtin_parity(ret) << 5;
    return ret;
}

static bool swd_read_request(SwdAccessType access_type, uint8_t address, uint32_t* data) {
    const uint8_t request = swd_prepare_request(SwdAccessDirectionRead, access_type, address);
    swd_tx(request, SWD_REQUEST_LEN);

    const uint32_t response = swd_rx(SWD_RESPONSE_LEN);
    if(response == SwdResponseOk) {
        return swd_rx_parity(data, SWD_DATA_LEN);
    } else {
        return false;
    }
}

static bool swd_write_request(SwdAccessType access_type, uint8_t address, uint32_t data) {
    const uint8_t request = swd_prepare_request(SwdAccessDirectionWrite, access_type, address);
    swd_tx(request, SWD_REQUEST_LEN);

    const uint32_t response = swd_rx(SWD_RESPONSE_LEN);
    if(response == SwdResponseOk) {
        swd_tx_parity(data, SWD_DATA_LEN);
        swd_tx(0UL, 8);
        return true;
    } else {
        return false;
    }
}

void swd_select_target(uint32_t target_id) {
    swd_tx(SWD_WRITE_REQUEST_INIT | (SWD_DP_REG_WO_TASRGETSEL << 3), SWD_REQUEST_LEN);
    swd_rx(SWD_RESPONSE_LEN);
    swd_tx_parity(target_id, SWD_DATA_LEN);
    swd_tx(0UL, 8);
}

bool swd_dp_read(uint8_t address, uint32_t* data) {
    return swd_read_request(SwdAccessTypeDp, address, data);
}

bool swd_dp_write(uint8_t address, uint32_t data) {
    return swd_write_request(SwdAccessTypeDp, address, data);
}

bool swd_ap_read(uint8_t address, uint32_t* data) {
    bool success = false;

    do {
        // Using hardcoded AP 0
        const uint32_t select_val = address & 0xF0U;
        if(!swd_write_request(SwdAccessTypeDp, SWD_DP_REG_WO_SELECT, select_val)) break;
        if(!swd_read_request(SwdAccessTypeAp, (address & 0x0FU) >> 2, NULL)) break;
        if(!swd_read_request(SwdAccessTypeDp, SWD_DP_REG_RO_RDBUFF, data)) break;
        success = true;
    } while(false);

    return success;
}

bool swd_ap_write(uint8_t address, uint32_t data) {
    bool success = false;

    do {
        // Using hardcoded AP 0
        const uint32_t select_val = address & 0xF0U;
        if(!swd_write_request(SwdAccessTypeDp, SWD_DP_REG_WO_SELECT, select_val)) break;
        if(!swd_write_request(SwdAccessTypeAp, (address & 0x0FU) >> 2, data)) break;
        success = true;
    } while(false);

    return success;
}

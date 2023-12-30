#pragma once

#include <stm32wbxx_ll_dma.h>

#include "led_driver.h"
#include "../app_config.h"

#define MAX_LED_COUNT 4

// We store the HIGH/LOW durations (2 values) for each color bit (24 bits per LED)
#define LED_DRIVER_BUFFER_SIZE (MAX_LED_COUNT * 2 * 24)

// We use a setinel value to figure out when the timer is complete.
#define LED_DRIVER_TIMER_SETINEL 0xFFU

/** 64 transitions per us @ 64MHz.  Our timing is in NANO_SECONDS */
#define LED_DRIVER_TIMER_NANOSECOND (1000U / (SystemCoreClock / 1000000U))

// Timings for WS2812B
#define LED_DRIVER_T0H 400U
#define LED_DRIVER_T1H 800U
#define LED_DRIVER_T0L 850U
#define LED_DRIVER_T1L 450U

// Max wait for the DMA to complete. NOTE: 4000 leds*(850ns+450ns)*24 = 124.8ms + 50ms blanking = 174.8ms
#define LED_DRIVER_SETINEL_WAIT_MS 200
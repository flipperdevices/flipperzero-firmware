
#ifndef WOLFSSL_ROWLEY_HW_H
#define WOLFSSL_ROWLEY_HW_H


#include <__cross_studio_io.h>
#include <__libc.h>
#include <stdint.h>

// Generic HW API
void hw_init(void);
uint32_t hw_get_time_sec(void);
uint32_t hw_get_time_msec(void);
void hw_uart_printchar(int c);
void hw_watchdog_disable(void);
uint32_t hw_rand(void);

// Delay functions
void delay_us(uint32_t microseconds);


#endif /* WOLFSSL_ROWLEY_HW_H */


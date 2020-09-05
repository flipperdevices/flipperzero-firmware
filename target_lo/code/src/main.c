#include <stdint.h>
#include <string.h>
#include <pthread.h>

#include <zephyr.h>
#include <device.h>
#include <drivers/gpio.h>
#include <sys/printk.h>
#include <cmsis_os2.h>

#include "main.h"

SerialDevice DEBUG_UART = NULL;

void app_serial_write(SerialDevice* serial, uint8_t* buffer, uint16_t size, uint32_t timeout) {
    write(1, (const char*)buffer, size);
}

bool task_equal(osThreadId_t a, osThreadId_t b) {
    if(a == NULL || b == NULL) return false;

    return a == b;
}

void app();

void main() {
    app();

    while(1);
}

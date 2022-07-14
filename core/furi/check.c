#include "check.h"
#include "common_defines.h"

#include <furi_hal_console.h>
#include <furi_hal_power.h>
#include <furi_hal_rtc.h>
#include <stdio.h>

#include <FreeRTOS.h>
#include <task.h>
#include <stdio.h>

extern size_t xPortGetTotalHeapSize(void);
extern size_t xPortGetFreeHeapSize(void);
extern size_t xPortGetMinimumEverFreeHeapSize(void);

static void __furi_print_stack_info() {
    char tmp_str[] = "-2147483648";
    itoa(uxTaskGetStackHighWaterMark(NULL) * 4, tmp_str, 10);
    furi_hal_console_puts("\r\n\tstack watermark: ");
    furi_hal_console_puts(tmp_str);
}

static void __furi_print_heap_info() {
    char tmp_str[] = "-2147483648";
    itoa(xPortGetTotalHeapSize(), tmp_str, 10);
    furi_hal_console_puts("\r\n\t     heap total: ");
    furi_hal_console_puts(tmp_str);

    itoa(xPortGetFreeHeapSize(), tmp_str, 10);
    furi_hal_console_puts("\r\n\t      heap free: ");
    furi_hal_console_puts(tmp_str);

    itoa(xPortGetMinimumEverFreeHeapSize(), tmp_str, 10);
    furi_hal_console_puts("\r\n\t heap watermark: ");
    furi_hal_console_puts(tmp_str);
}

static void __furi_print_name(bool isr) {
    if(isr) {
        char tmp_str[] = "-2147483648";
        itoa(__get_IPSR(), tmp_str, 10);
        furi_hal_console_puts("[ISR ");
        furi_hal_console_puts(tmp_str);
        furi_hal_console_puts("] ");
    } else {
        const char* name = pcTaskGetName(xTaskGetCurrentTaskHandle());
        if(name == NULL) {
            furi_hal_console_puts("[main] ");
        } else {
            furi_hal_console_puts("[");
            furi_hal_console_puts(name);
            furi_hal_console_puts("] ");
        }
    }
}

static FURI_NORETURN void __furi_halt() {
    asm volatile(
#ifdef FURI_DEBUG
        "bkpt 0x00  \n"
#endif
        "loop%=:    \n"
        "wfi        \n"
        "b loop%=   \n"
        :
        :
        : "memory");
    __builtin_unreachable();
}

FURI_NORETURN void furi_crash(const char* message) {
    bool isr = FURI_IS_ISR();
    __disable_irq();

    if(message == NULL) {
        message = "Fatal Error";
    }

    furi_hal_console_puts("\r\n\033[0;31m[CRASH]");
    __furi_print_name(isr);
    furi_hal_console_puts(message);

    if(!isr) {
        __furi_print_stack_info();
    }
    __furi_print_heap_info();

#ifdef FURI_DEBUG
    furi_hal_console_puts("\r\nSystem halted. Connect debugger for more info\r\n");
    furi_hal_console_puts("\033[0m\r\n");
    __furi_halt();
#else
    furi_hal_rtc_set_fault_data((uint32_t)message);
    furi_hal_console_puts("\r\nRebooting system.\r\n");
    furi_hal_console_puts("\033[0m\r\n");
    furi_hal_power_reset();
#endif
    __builtin_unreachable();
}

FURI_NORETURN void furi_halt(const char* message) {
    bool isr = FURI_IS_ISR();
    __disable_irq();

    if(message == NULL) {
        message = "System halt requested.";
    }

    furi_hal_console_puts("\r\n\033[0;31m[HALT]");
    __furi_print_name(isr);
    furi_hal_console_puts(message);
    furi_hal_console_puts("\r\nSystem halted. Bye-bye!\r\n");
    furi_hal_console_puts("\033[0m\r\n");
    __furi_halt();
}

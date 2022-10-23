/**
 * @file check.h
 * 
 * Furi crash and assert functions.
 * 
 * The main problem with crashing is that you can't do anything without disturbing registers, 
 * and if you disturb registers, you won't be able to see the correct register values in the debugger.
 * 
 * So, we use the trick: we keep the crash message in register r12 (scratch register), 
 * and then we call the noreturn crash function, which saves the registers r0-r11 in the memory location, 
 * outputs a crash message, restores the registers and halts the MCU.=
 * 
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#define FURI_NORETURN [[noreturn]]
#else
#include <stdnoreturn.h>
#define FURI_NORETURN noreturn
#endif

/** Crash system */
FURI_NORETURN void __furi_crash();

/** Halt system */
FURI_NORETURN void __furi_halt();

/** Crash system with message. Show message after reboot. */
#define furi_crash(message)                                   \
    do {                                                      \
        register const void* r12 asm("r12") = (void*)message; \
        asm volatile("sukima%=:" : : "r"(r12));               \
        __furi_crash();                                       \
    } while(0)

/** Halt system with message. */
#define furi_halt(message)                                    \
    do {                                                      \
        register const void* r12 asm("r12") = (void*)message; \
        asm volatile("sukima%=:" : : "r"(r12));               \
        __furi_halt();                                        \
    } while(0)

/** Check condition and crash if check failed */
#define furi_check(__e)                          \
    do {                                         \
        if((__e) == 0) {                         \
            furi_crash("furi_check failed\r\n"); \
        }                                        \
    } while(0)

/** Only in debug build: Assert condition and crash if assert failed  */
#ifdef FURI_DEBUG
#define furi_assert(__e)                          \
    do {                                          \
        if((__e) == 0) {                          \
            furi_crash("furi_assert failed\r\n"); \
        }                                         \
    } while(0)
#else
#define furi_assert(__e) \
    do {                 \
        ((void)(__e));   \
    } while(0)
#endif

#ifdef __cplusplus
}
#endif

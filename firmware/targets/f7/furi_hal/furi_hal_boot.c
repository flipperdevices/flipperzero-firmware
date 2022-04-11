#include <furi_hal_boot.h>
#include <furi_hal_rtc.h>
#include <furi.h>

#define TAG "FuriHalBoot"

// Boot request enum
#define BOOT_REQUEST_TAINTED 0x00000000
#define BOOT_REQUEST_CLEAN 0xDADEDADE
#define BOOT_REQUEST_DFU 0xDF00B000

void furi_hal_boot_init() {
#ifndef DEBUG
    furi_hal_rtc_set_register(FuriHalRtcRegisterBoot, BOOT_REQUEST_TAINTED);
#endif
    FURI_LOG_I(TAG, "Init OK");
}

void furi_hal_boot_switch(void* address) {
    asm volatile("ldr    r3, [%0]    \n"
                 "msr    msp, r3     \n"
                 "ldr    r3, [%1]    \n"
                 "mov    pc, r3      \n"
                 :
                 : "r"(address), "r"(address + 0x4)
                 : "r3");
}

void furi_hal_boot_set_mode(FuriHalBootMode mode) {
    if(mode == FuriHalBootModeNormal) {
        furi_hal_rtc_set_register(FuriHalRtcRegisterBoot, BOOT_REQUEST_CLEAN);
    } else if(mode == FuriHalBootModeDFU) {
        furi_hal_rtc_set_register(FuriHalRtcRegisterBoot, BOOT_REQUEST_DFU);
    }
}

FuriHalBootMode furi_hal_boot_get_mode() {
    uint32_t bootraw = furi_hal_rtc_get_register(FuriHalRtcRegisterBoot);
    switch(bootraw) {
    case BOOT_REQUEST_DFU:
        return FuriHalBootModeDFU;
    default:
        return FuriHalBootModeNormal;
    }
}

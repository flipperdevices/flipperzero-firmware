#pragma once

#ifdef __cplusplus
extern "C" {
#endif

static inline void flipper_boot_target_switch(void* offset) {
    asm volatile("ldr    r3, [%0]    \n"
                 "msr    msp, r3     \n"
                 "ldr    r3, [%1]    \n"
                 "mov    pc, r3      \n"
                 :
                 : "r"(offset), "r"(offset + 0x4)
                 : "r3");
}

void flipper_boot_update_exec();

void flipper_boot_dfu_exec();

#ifdef __cplusplus
}
#endif
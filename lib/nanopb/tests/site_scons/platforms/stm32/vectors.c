#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

extern void _start();
extern void* __StackTop;

static void HardFaultHandler()
{
    uint32_t args[3];
    args[0] = 2;
    args[1] = (uint32_t)"HARDFAULT";
    args[2] = 9;
    
    asm("mov r0, #5\n"
        "mov r1, %0\n"
        "bkpt 0x00ab"  : : "r"(args) : "r0", "r1", "memory");
    asm("mov r12, %0\n" "mov r0, #24\n" "bkpt 0x00ab" : : "r"(0xDEADBEEF) : "r0");
    while(1);
}

void* const g_vector_table[16] __attribute__((section(".isr_vector"))) = {
    (void*)&__StackTop,
    (void*)&_start,
    (void*)&HardFaultHandler,
    (void*)&HardFaultHandler,
    (void*)&HardFaultHandler,
    (void*)&HardFaultHandler,
    (void*)&HardFaultHandler,
};


void ramboot() __attribute__((noreturn, naked, section(".ramboot")));
void ramboot()
{
    *(const void**)0xE000ED08 = g_vector_table; // SCB->VTOR
      __asm__(
    "msr msp, %0\n\t"
    "bx %1" : : "r" (g_vector_table[0]),
                "r" (g_vector_table[1]) : "memory");
}

#ifdef __cplusplus
}
#endif

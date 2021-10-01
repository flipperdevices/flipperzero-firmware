ENTRY(Reset_Handler)
MEMORY
{
    ROM     (rx): ORIGIN = 0x08000000, LENGTH = 512K
    RAM    (rwx): ORIGIN = 0x20000000, LENGTH = 64K
    CCMRAM (rwx): ORIGIN = 0x10000000, LENGTH = 16K
}

INCLUDE sections.ld

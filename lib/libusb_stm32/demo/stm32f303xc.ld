ENTRY(Reset_Handler)
MEMORY
{
    ROM     (rx): ORIGIN = 0x08000000, LENGTH = 256K
    RAM    (rwx): ORIGIN = 0x20000000, LENGTH = 40K
    CCMRAM (rwx): ORIGIN = 0x10000000, LENGTH = 8K
}

INCLUDE sections.ld

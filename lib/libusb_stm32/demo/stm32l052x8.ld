ENTRY(Reset_Handler)
MEMORY
{
    ROM  (rx): ORIGIN = 0x08000000, LENGTH =  64K
    RAM (rwx): ORIGIN = 0x20000000, LENGTH =  8K
}

INCLUDE sections.ld

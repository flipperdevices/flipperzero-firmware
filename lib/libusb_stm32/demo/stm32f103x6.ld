ENTRY(Reset_Handler)
MEMORY
{
    ROM  (rx): ORIGIN = 0x08000000, LENGTH =  32K
    RAM (rwx): ORIGIN = 0x20000000, LENGTH =  10K
}

INCLUDE sections.ld

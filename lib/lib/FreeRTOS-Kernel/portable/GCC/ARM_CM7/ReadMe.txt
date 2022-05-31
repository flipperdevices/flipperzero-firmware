There are two options for running FreeRTOS on ARM Cortex-M7 microcontrollers.
The best option depends on the revision of the ARM Cortex-M7 core in use.  The
revision is specified by an 'r' number, and a 'p' number, so will look something
like 'r0p1'.  Check the documentation for the microcontroller in use to find the 
revision of the Cortex-M7 core used in that microcontroller.  If in doubt, use 
the FreeRTOS port provided specifically for r0p1 revisions, as that can be used
with all core revisions.

The first option is to use the ARM Cortex-M4F port, and the second option is to
use the Cortex-M7 r0p1 port - the latter containing a minor errata workaround.

If the revision of the ARM Cortex-M7 core is not r0p1 then either option can be
used, but it is recommended to use the FreeRTOS ARM Cortex-M4F port located in 
the /FreeRTOS/Source/portable/GCC/ARM_CM4F directory.

If the revision of the ARM Cortex-M7 core is r0p1 then use the FreeRTOS ARM
Cortex-M7 r0p1 port located in the /FreeRTOS/Source/portable/GCC/ARM_CM7/r0p1
directory.
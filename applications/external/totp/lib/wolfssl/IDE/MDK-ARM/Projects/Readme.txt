
Use appropriate config file for the target library.

Configfile files               Target
config-WOLFLIB.h:         wolfSSL-Lib                  /* for general use wolfSSL library */
config-BARE-METAL.h:  MDK-BAREMETAL-Lib   /* for linking with MDK-BAREMETAL target in MDK-ARM-STM32F2xx project */
config-FS.h:                  MDK-FS-Lib                 /* for linking with MDK-FS target in MDK-ARM-STM32F2xx project */
config-RTX-TCP-FS.h:  MDK-RTX-TCP-FS-Lib  /* for linking with MDK-RTX-TCP-FS target in MDK-ARM-STM32F2xx project */

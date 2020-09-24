/*
Flipper devices inc.

Boot control quirks
*/

#ifndef __BOOT_H
#define __BOOT_H

/*
 * @brief On OS boot hook, check's if dfu-mode requested.
 * Passes control to boot loader or returns to system init.
*/
void on_os_boot();

/*
 * @brief request dfu and reboot.
*/
void restart_in_dfu();

#endif

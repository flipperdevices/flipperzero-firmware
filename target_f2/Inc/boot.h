/*
Flipper devices inc.

Boot control quirks
*/

#ifndef __BOOT_H
#define __BOOT_H

/*
 * @brief Before hardware initizliation hook
*/
void boot_before_hw_hook();

/*
 * @brief After hardware initialization hook
*/
void boot_after_hw_hook();

/*
 * @brief Request DFU and reboot
*/
void boot_restart_in_dfu();

#endif

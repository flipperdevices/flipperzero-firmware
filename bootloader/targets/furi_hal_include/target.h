#ifndef TARGET_H
#define TARGET_H

/**
 * Initialize hardware
*/
void target_init();

typedef enum { BootMode_Normal, BootMode_DFU, BootMode_SDUpdate } BootMode;

/**
 * Get desired boot mode
 * @return 
 */
BootMode target_get_boot_mode();

/**
 * Switch to dfu mode
 */
void target_switch2dfu();

/**
 * Switch to OS
 */
void target_switch2os();

/**
 * Switch to SD Update
 */
void target_switch2sdupdate();

#endif
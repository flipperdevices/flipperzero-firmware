#ifndef PLATFORM_H
#define PLATFORM_H

/*
 * Initialize hardware
*/
void platform_init();

/*
 * Check if dfu mode requested
*/
int platform_is_dfu_requested();

/*
 * Switch to bootloader
*/
void platform_switch2boot();

/*
 * Switch to OS
*/
void platform_switch2os();


#endif
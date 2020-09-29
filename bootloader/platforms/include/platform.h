#ifndef PLATFORM_H
#define PLATFORM_H

/*
 * Initialize hardware
*/
void platform_init();

/*
 * Check if dfu mode requested
 * @return 1 if dfu mode requested, 0 if not
*/
int platform_is_dfu_requested();

/*
 * Switch to dfu mode
*/
void platform_switch2dfu();

/*
 * Switch to OS
*/
void platform_switch2os();


#endif
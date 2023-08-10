/* main.h */

#ifndef __main_h_
#define __main_h_

#include <mqx.h>
#include <bsp.h>
#include <fio.h>
#include <mfs.h>
#include <sdcard.h>
#include <spi.h>
#include <part_mgr.h>

#define MAIN_TASK   1

extern void Main_task (uint32_t);
void wolfcrypt_test(void* args);

#endif /* __main_h_ */


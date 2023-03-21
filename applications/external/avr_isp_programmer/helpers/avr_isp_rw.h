#pragma once

#include <furi_hal.h>

typedef struct AvrIspRW AvrIspRW;

typedef void (*AvrIspRWCallback)(void* context, const char* name, bool detect_chip, uint32_t flash_size);

/** Allocate AvrIspRW
 * 
 * @param context AvrIsp* context
 * @return AvrIspRW* 
 */
AvrIspRW* avr_isp_rw_alloc(void* context);

/** Free AvrIspRW
 * 
 * @param instance AvrIspRW instance
 */
void avr_isp_rw_free(AvrIspRW* instance);

/** Callback AvrIspRW
 *
 * @param instance AvrIspRW instance
 * @param callback AvrIspRWOverrunCallback callback
 * @param context
 */
void avr_isp_rw_set_callback(AvrIspRW* instance, AvrIspRWCallback callback, void* context);

/** Detect chip
 * 
 * @param instance AvrIspRW instance
 * @return bool 
 */
bool avr_isp_rw_detect_chip(AvrIspRW* instance);

bool avr_isp_rw_get_dump(AvrIspRW* instance, const char* file_path, const char* file_name);

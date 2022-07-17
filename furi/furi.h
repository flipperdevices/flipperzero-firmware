#pragma once

#include <FreeRTOS.h>
#include <timers.h>
#include <task.h>

#include <cmsis_os2.h>

#include <core/check.h>
#include <core/common_defines.h>
#include <core/log.h>
#include <core/event_flags.h>
#include <core/memmgr.h>
#include <core/memmgr_heap.h>
#include <core/mutex.h>
#include <core/pubsub.h>
#include <core/record.h>
#include <core/semaphore.h>
#include <core/stdglue.h>
#include <core/thread.h>
#include <core/valuemutex.h>

#include <furi_hal_gpio.h>

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

void furi_init();

void furi_run();

#ifdef __cplusplus
}
#endif

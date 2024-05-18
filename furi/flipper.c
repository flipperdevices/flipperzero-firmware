#include "flipper.h"
#include <applications.h>
#include <furi.h>
#include <furi_hal.h>
#include <furi_hal_version.h>
#include <furi_hal_memory.h>
#include <furi_hal_rtc.h>
#include <storage/storage.h>
#include <gui/canvas_i.h>
#include <cfw/private.h>

#include <FreeRTOS.h>

#define TAG "Flipper"

static void flipper_print_version(const char* target, const Version* version) {
    if(version) {
        FURI_LOG_I(
            TAG,
            "\r\n\t%s version:\t%s\r\n"
            "\tBuild date:\t\t%s\r\n"
            "\tGit Commit:\t\t%s (%s)%s\r\n"
            "\tGit Branch:\t\t%s",
            target,
            version_get_version(version),
            version_get_builddate(version),
            version_get_githash(version),
            version_get_gitbranchnum(version),
            version_get_dirty_flag(version) ? " (dirty)" : "",
            version_get_gitbranch(version));
    } else {
        FURI_LOG_I(TAG, "No build info for %s", target);
    }
}

void flipper_start_service(const FlipperInternalApplication* service) {
    FURI_LOG_D(TAG, "Starting service %s", service->name);

    FuriThread* thread =
        furi_thread_alloc_ex(service->name, service->stack_size, service->app, NULL);
    furi_thread_mark_as_service(thread);
    furi_thread_set_appid(thread, service->appid);

    furi_thread_start(thread);
}

void flipper_init(void) {
    flipper_print_version("Firmware", furi_hal_version_get_firmware_version());
    FURI_LOG_I(TAG, "Boot mode %d, starting services", furi_hal_rtc_get_boot_mode());

    for(size_t i = 0; i < FLIPPER_SERVICES_COUNT; i++) {
        flipper_start_service(&FLIPPER_SERVICES[i]);
    }
    if(furi_hal_is_normal_boot()) {
        cfw_settings_load();
    } else {
        FURI_LOG_I(TAG, "Special boot, skipping optional components");
    }

    FURI_LOG_I(TAG, "Startup complete");
}

PLACE_IN_SECTION("MB_MEM2") static StaticTask_t idle_task_tcb;
PLACE_IN_SECTION("MB_MEM2") static StackType_t idle_task_stack[configIDLE_TASK_STACK_DEPTH];
PLACE_IN_SECTION("MB_MEM2") static StaticTask_t timer_task_tcb;
PLACE_IN_SECTION("MB_MEM2") static StackType_t timer_task_stack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetIdleTaskMemory(
    StaticTask_t** tcb_ptr,
    StackType_t** stack_ptr,
    uint32_t* stack_size) {
    *tcb_ptr = &idle_task_tcb;
    *stack_ptr = idle_task_stack;
    *stack_size = configIDLE_TASK_STACK_DEPTH;
}

void vApplicationGetTimerTaskMemory(
    StaticTask_t** tcb_ptr,
    StackType_t** stack_ptr,
    uint32_t* stack_size) {
    *tcb_ptr = &timer_task_tcb;
    *stack_ptr = timer_task_stack;
    *stack_size = configTIMER_TASK_STACK_DEPTH;
}

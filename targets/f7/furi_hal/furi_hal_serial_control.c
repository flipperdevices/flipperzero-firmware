#include "furi_hal_serial_control.h"
#include "furi_hal_serial.h"
#include <furi.h>
#include <toolbox/api_lock.h>

#define TAG "FuriHalSerialControl"

typedef enum {
    FuriHalSerialControlMessageTypeStop,
    FuriHalSerialControlMessageTypeSuspend,
    FuriHalSerialControlMessageTypeResume,
    FuriHalSerialControlMessageTypeAcquire,
    FuriHalSerialControlMessageTypeRelease,
} FuriHalSerialControlMessageType;

typedef struct {
    FuriHalSerialControlMessageType type;
    FuriApiLock api_lock;
    void* input;
    void* output;
} FuriHalSerialControlMessage;

typedef struct {
    FuriHalSerialHandle handles[FuriHalUartIdMax];
    FuriMessageQueue* queue;
    FuriThread* thread;
    FuriLogHandler log_handler;
} FuriHalSerialControl;

FuriHalSerialControl* furi_hal_serial_control = NULL;

static int32_t furi_hal_serial_control_thread(void* args) {
    UNUSED(args);

    bool should_continue = true;
    while(should_continue || furi_message_queue_get_count(furi_hal_serial_control->queue) > 0) {
        FuriHalSerialControlMessage message = {0};
        FuriStatus status =
            furi_message_queue_get(furi_hal_serial_control->queue, &message, FuriWaitForever);
        furi_check(status == FuriStatusOk);

        if(message.type == FuriHalSerialControlMessageTypeStop) {
            should_continue = false;
        } else if(message.type == FuriHalSerialControlMessageTypeSuspend) {
            for(size_t i = 0; i < FuriHalUartIdMax; i++) {
                furi_hal_serial_tx_wait_complete(&furi_hal_serial_control->handles[i]);
                furi_hal_serial_suspend(&furi_hal_serial_control->handles[i]);
            }
            api_lock_unlock(message.api_lock);
        } else if(message.type == FuriHalSerialControlMessageTypeResume) {
            for(size_t i = 0; i < FuriHalUartIdMax; i++) {
                furi_hal_serial_resume(&furi_hal_serial_control->handles[i]);
            }
            api_lock_unlock(message.api_lock);
        } else if(message.type == FuriHalSerialControlMessageTypeAcquire) {
            FuriHalUartId uart_id = *(FuriHalUartId*)message.input;
            if(furi_hal_serial_control->handles[uart_id].is_used) {
                *(FuriHalSerialHandle**)message.output = NULL;
            } else {
                // Disable logging
                if(uart_id == FuriHalUartIdUSART1) {
                    furi_log_remove_handler(furi_hal_serial_control->log_handler);
                    furi_hal_serial_deinit(&furi_hal_serial_control->handles[FuriHalUartIdUSART1]);
                }
                // Return handle
                furi_hal_serial_control->handles[uart_id].is_used = true;
                *(FuriHalSerialHandle**)message.output =
                    &furi_hal_serial_control->handles[uart_id];
            }
            api_lock_unlock(message.api_lock);
        } else if(message.type == FuriHalSerialControlMessageTypeRelease) {
            FuriHalSerialHandle* handle = *(FuriHalSerialHandle**)message.input;
            furi_assert(handle->is_used);
            furi_hal_serial_deinit(handle);
            handle->is_used = false;
            // Return back logging
            if(handle->id == FuriHalUartIdUSART1) {
                furi_hal_serial_init(
                    &furi_hal_serial_control->handles[FuriHalUartIdUSART1], 230400);
                furi_log_add_handler(furi_hal_serial_control->log_handler);
            }
            api_lock_unlock(message.api_lock);
        } else {
            furi_crash("Invalid parameter");
        }
    }

    return 0;
}

static void furi_hal_serial_control_log_callback(const uint8_t* data, size_t size, void* context) {
    FuriHalSerialHandle* handle = context;
    furi_hal_serial_tx(handle, data, size);
}

void furi_hal_serial_control_init(void) {
    furi_check(furi_hal_serial_control == NULL);
    // Allocate resources
    furi_hal_serial_control = malloc(sizeof(FuriHalSerialControl));
    furi_hal_serial_control->handles[FuriHalUartIdUSART1].id = FuriHalUartIdUSART1;
    furi_hal_serial_control->queue =
        furi_message_queue_alloc(8, sizeof(FuriHalSerialControlMessage));
    furi_hal_serial_control->thread =
        furi_thread_alloc_ex("SerialControlDriver", 512, furi_hal_serial_control_thread, NULL);
    furi_thread_mark_as_service(furi_hal_serial_control->thread);
    furi_thread_set_priority(furi_hal_serial_control->thread, FuriThreadPriorityHighest);
    // Logging handle
    furi_hal_serial_init(&furi_hal_serial_control->handles[FuriHalUartIdUSART1], 230400);
    furi_hal_serial_control->log_handler.callback = furi_hal_serial_control_log_callback;
    furi_hal_serial_control->log_handler.context =
        &furi_hal_serial_control->handles[FuriHalUartIdUSART1];
    furi_log_add_handler(furi_hal_serial_control->log_handler);
    // Start control plane thread
    furi_thread_start(furi_hal_serial_control->thread);
}

void furi_hal_serial_control_deinit(void) {
    furi_check(furi_hal_serial_control);
    // Stop control plane thread
    FuriHalSerialControlMessage message;
    message.type = FuriHalSerialControlMessageTypeStop;
    furi_message_queue_put(furi_hal_serial_control->queue, &message, FuriWaitForever);
    furi_thread_join(furi_hal_serial_control->thread);
    // Release resources
    furi_thread_free(furi_hal_serial_control->thread);
    furi_message_queue_free(furi_hal_serial_control->queue);
    free(furi_hal_serial_control);
}

void furi_hal_serial_control_suspend(void) {
    furi_check(furi_hal_serial_control);

    FuriHalSerialControlMessage message;
    message.type = FuriHalSerialControlMessageTypeSuspend;
    message.api_lock = api_lock_alloc_locked();
    furi_message_queue_put(furi_hal_serial_control->queue, &message, FuriWaitForever);
    api_lock_wait_unlock_and_free(message.api_lock);
}

void furi_hal_serial_control_resume(void) {
    furi_check(furi_hal_serial_control);

    FuriHalSerialControlMessage message;
    message.type = FuriHalSerialControlMessageTypeResume;
    message.api_lock = api_lock_alloc_locked();
    furi_message_queue_put(furi_hal_serial_control->queue, &message, FuriWaitForever);
    api_lock_wait_unlock_and_free(message.api_lock);
}

FuriHalSerialHandle* furi_hal_serial_control_acquire(FuriHalUartId uart_id) {
    furi_check(furi_hal_serial_control);

    FuriHalSerialHandle* output = NULL;

    FuriHalSerialControlMessage message;
    message.type = FuriHalSerialControlMessageTypeAcquire;
    message.api_lock = api_lock_alloc_locked();
    message.input = &uart_id;
    message.output = &output;
    furi_message_queue_put(furi_hal_serial_control->queue, &message, FuriWaitForever);
    api_lock_wait_unlock_and_free(message.api_lock);

    return output;
}

void furi_hal_serial_control_release(FuriHalSerialHandle* handle) {
    furi_check(furi_hal_serial_control);
    furi_check(handle);

    FuriHalSerialControlMessage message;
    message.type = FuriHalSerialControlMessageTypeRelease;
    message.api_lock = api_lock_alloc_locked();
    message.input = &handle;
    furi_message_queue_put(furi_hal_serial_control->queue, &message, FuriWaitForever);
    api_lock_wait_unlock_and_free(message.api_lock);
}

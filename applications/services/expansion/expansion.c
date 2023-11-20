#include "expansion.h"

#include <furi_hal_serial.h>
#include <furi_hal_serial_control.h>

#include <furi.h>

#include "expansion_settings.h"
#include "expansion_frame.h"

#define TAG "ExpansionSrv"

typedef enum {
    ExpansionStateDisabled,
    ExpansionStateEnabled,
    ExpansionStateRunning,
} ExpansionState;

typedef enum {
    ExpansionFlagStop = 1 << 0,
    ExpansionFlagData = 1 << 1,
} ExpansionFlag;

#define EXPANSION_ALL_FLAGS (ExpansionFlagData | ExpansionFlagStop)

struct Expansion {
    ExpansionState state;
    FuriMutex* state_mutex;
    FuriThread* thread;
    FuriStreamBuffer* buf;
    FuriHalSerialId serial_id;
};

static void expansion_serial_rx_callback(uint8_t data, void* context) {
    furi_assert(context);
    Expansion* instance = context;

    furi_stream_buffer_send(instance->buf, &data, sizeof(data), 0);
    furi_thread_flags_set(furi_thread_get_id(instance->thread), ExpansionFlagData);
}

static int32_t expansion_worker(void* context) {
    furi_assert(context);
    Expansion* instance = context;

    furi_hal_serial_control_set_expansion_callback(instance->serial_id, NULL, NULL);

    FuriHalSerialHandle* handle = furi_hal_serial_control_acquire(instance->serial_id);

    if(!handle) {
        // TODO: Some error recovery behaviour
    }

    furi_hal_serial_init(handle, 230400);
    furi_hal_serial_set_rx_callback(handle, expansion_serial_rx_callback, instance);

    while(true) {
        // Temp echo
        uint8_t data;
        const size_t n_received =
            furi_stream_buffer_receive(instance->buf, &data, sizeof(data), 0);
        if(n_received == sizeof(data)) {
            furi_hal_serial_tx(handle, &data, 1);
        }

        const uint32_t flags =
            furi_thread_flags_wait(EXPANSION_ALL_FLAGS, FuriFlagWaitAny, FuriWaitForever);

        if(flags & ExpansionFlagStop) {
            if(furi_stream_buffer_is_empty(instance->buf)) {
                break;
            } else {
                furi_thread_flags_set(furi_thread_get_current_id(), ExpansionFlagStop);
            }
        }
    }

    furi_hal_serial_control_release(handle);

    return 0;
}

static void expansion_timer_pending_callback(void* context, uint32_t arg) {
    UNUSED(arg);
    furi_assert(context);
    Expansion* instance = context;

    FURI_LOG_D(TAG, "Worker is stopping!");

    furi_stream_buffer_free(instance->buf);

    furi_thread_join(instance->thread);
    furi_thread_free(instance->thread);
}

static void expansion_worker_state_callback(FuriThreadState state, void* context) {
    if(state == FuriThreadStateStopped) {
        furi_timer_pending_callback(expansion_timer_pending_callback, context, 0);
    }
}

static void expansion_detect_callback(void* context) {
    furi_assert(context);
    Expansion* instance = context;

    furi_mutex_acquire(instance->state_mutex, FuriWaitForever);

    if(instance->state == ExpansionStateEnabled) {
        instance->thread = furi_thread_alloc_ex(TAG, 1024, expansion_worker, instance);
        instance->buf = furi_stream_buffer_alloc(64, 1);

        furi_thread_set_state_callback(instance->thread, expansion_worker_state_callback);
        furi_thread_set_state_context(instance->thread, instance);
        furi_thread_start(instance->thread);

        instance->state = ExpansionStateRunning;
    }

    furi_mutex_release(instance->state_mutex);
}

void expansion_enable(Expansion* instance, FuriHalSerialId serial_id) {
    furi_mutex_acquire(instance->state_mutex, FuriWaitForever);

    if(instance->state == ExpansionStateRunning) {
        furi_thread_flags_set(furi_thread_get_id(instance->thread), ExpansionFlagStop);
    }

    instance->serial_id = serial_id;
    instance->state = ExpansionStateEnabled;

    furi_mutex_release(instance->state_mutex);
    furi_hal_serial_control_set_expansion_callback(serial_id, expansion_detect_callback, instance);
}

void expansion_disable(Expansion* instance) {
    furi_mutex_acquire(instance->state_mutex, FuriWaitForever);

    if(instance->state == ExpansionStateRunning) {
        furi_thread_flags_set(furi_thread_get_id(instance->thread), ExpansionFlagStop);
    }

    instance->state = ExpansionStateDisabled;

    furi_mutex_release(instance->state_mutex);
    furi_hal_serial_control_set_expansion_callback(instance->serial_id, NULL, NULL);
}

static Expansion* expansion_alloc() {
    Expansion* instance = malloc(sizeof(Expansion));
    instance->state_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    return instance;
}

void expansion_on_system_start(void* arg) {
    UNUSED(arg);

    Expansion* instance = expansion_alloc();
    furi_record_create(RECORD_EXPANSION, instance);

    ExpansionSettings settings = {};
    if(!expansion_settings_load(&settings)) {
        expansion_settings_save(&settings);
    } else if(settings.uart_index != 0) {
        expansion_enable(instance, settings.uart_index - 1);
    }
}

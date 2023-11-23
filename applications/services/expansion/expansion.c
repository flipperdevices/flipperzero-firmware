#include "expansion.h"

#include <furi_hal_serial.h>
#include <furi_hal_serial_control.h>

#include <furi.h>

#include <rpc/rpc.h>

#include "expansion_settings.h"
#include "expansion_protocol.h"

#define TAG "ExpansionSrv"

typedef enum {
    ExpansionStateDisabled,
    ExpansionStateEnabled,
    ExpansionStateRunning,
} ExpansionState;

typedef enum {
    ExpansionSessionStateHandShake,
    ExpansionSessionStateNormal,
    ExpansionSessionStateRpc,
} ExpansionSessionState;

typedef enum {
    ExpansionFlagStop = 1 << 0,
    ExpansionFlagData = 1 << 1,
} ExpansionFlag;

#define EXPANSION_ALL_FLAGS (ExpansionFlagData | ExpansionFlagStop)

struct Expansion {
    ExpansionState state;
    ExpansionSessionState session_state;
    ExpansionFrame frame;
    FuriMutex* state_mutex;
    FuriThread* thread;
    FuriStreamBuffer* buf;
    FuriHalSerialId serial_id;
    FuriHalSerialHandle* handle;
    RpcSession* rpc_session;
};

static void expansion_serial_rx_callback(uint8_t data, void* context) {
    furi_assert(context);
    Expansion* instance = context;

    furi_stream_buffer_send(instance->buf, &data, sizeof(data), 0);
    furi_thread_flags_set(furi_thread_get_id(instance->thread), ExpansionFlagData);
}

static size_t expansion_receive_callback(uint8_t* data, size_t data_size, void* context) {
    Expansion* instance = context;

    size_t received_size = 0;

    while(true) {
        received_size += furi_stream_buffer_receive(
            instance->buf, data + received_size, data_size - received_size, 0);
        if(received_size == data_size) break;

        const uint32_t flags =
            furi_thread_flags_wait(EXPANSION_ALL_FLAGS, FuriFlagWaitAny, FuriWaitForever);
        // TODO: Some way to exit out of this function on request
        UNUSED(flags);
    }

    return received_size;
}

static size_t expansion_send_callback(const uint8_t* data, size_t data_size, void* context) {
    Expansion* instance = context;

    furi_hal_serial_tx(instance->handle, data, data_size);

    return data_size;
}

static void expansion_send_status_response(Expansion* instance, ExpansionFrameError error) {
    ExpansionFrame frame = {
        .header.type = ExpansionFrameTypeStatus,
        .content.status.error = error,
    };

    expansion_frame_encode(&frame, expansion_send_callback, instance);
}

static void
    expansion_send_data_response(Expansion* instance, const uint8_t* data, size_t data_size) {
    furi_assert(data_size <= EXPANSION_MAX_DATA_SIZE);

    ExpansionFrame frame = {
        .header.type = ExpansionFrameTypeData,
        .content.data.size = data_size,
    };

    memcpy(frame.content.data.bytes, data, data_size);
    expansion_frame_encode(&frame, expansion_send_callback, instance);
}

static void expansion_rpc_send_callback(void* context, uint8_t* data, size_t data_size) {
    // TODO: split big data across several packets
    furi_assert(data_size <= EXPANSION_MAX_DATA_SIZE);
    Expansion* instance = context;
    expansion_send_data_response(instance, data, data_size);
}

static int32_t expansion_worker(void* context) {
    furi_assert(context);
    Expansion* instance = context;

    furi_hal_serial_control_set_expansion_callback(instance->serial_id, NULL, NULL);

    instance->handle = furi_hal_serial_control_acquire(instance->serial_id);

    if(!instance->handle) {
        // TODO: Some error recovery behaviour
    }

    furi_hal_serial_init(instance->handle, EXPANSION_DEFAULT_BAUD_RATE);
    furi_hal_serial_set_rx_callback(instance->handle, expansion_serial_rx_callback, instance);

    instance->session_state = ExpansionSessionStateHandShake;

    while(true) {
        if(!expansion_frame_decode(&instance->frame, expansion_receive_callback, instance)) {
            // TODO: Better error behaviour?
            break;
        }

        const ExpansionFrameType frame_type = instance->frame.header.type;

        switch(instance->session_state) {
        case ExpansionSessionStateHandShake:
            if(frame_type == ExpansionFrameTypeBaudRate) {
                // TODO: proper baud rate check
                if(instance->frame.content.baud_rate.baud == 230400) {
                    // Send response on previous baud rate
                    expansion_send_status_response(instance, ExpansionFrameErrorNone);
                    // Set new baud rate
                    furi_hal_serial_set_br(instance->handle, 230400);
                    instance->session_state = ExpansionSessionStateNormal;
                    break;
                }
            }

            expansion_send_status_response(instance, ExpansionFrameErrorUnknown);

            break;
        case ExpansionSessionStateNormal:
            if(frame_type == ExpansionFrameTypeControl) {
                if(instance->frame.content.control.command ==
                   ExpansionFrameControlCommandStartRpc) {
                    Rpc* rpc = furi_record_open(RECORD_RPC);
                    instance->rpc_session = rpc_session_open(rpc, RpcOwnerUnknown);
                    instance->session_state = ExpansionSessionStateRpc;

                    rpc_session_set_context(instance->rpc_session, instance);
                    rpc_session_set_send_bytes_callback(
                        instance->rpc_session, expansion_rpc_send_callback);

                    expansion_send_status_response(instance, ExpansionFrameErrorNone);
                    break;
                }
            }

            expansion_send_status_response(instance, ExpansionFrameErrorUnknown);

            break;
        case ExpansionSessionStateRpc:
            if(frame_type == ExpansionFrameTypeData) {
                // TODO: check the amount of bytes actually fed
                rpc_session_feed(
                    instance->rpc_session,
                    instance->frame.content.data.bytes,
                    instance->frame.content.data.size,
                    1000);
                break;
            } else if(frame_type == ExpansionFrameTypeControl) {
                if(instance->frame.content.control.command ==
                   ExpansionFrameControlCommandStopRpc) {
                    rpc_session_close(instance->rpc_session);
                    instance->session_state = ExpansionSessionStateNormal;
                    expansion_send_status_response(instance, ExpansionFrameErrorNone);
                    break;
                }
            }

            expansion_send_status_response(instance, ExpansionFrameErrorUnknown);
            break;
        default:
            break;
        }
    }

    furi_hal_serial_control_release(instance->handle);

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

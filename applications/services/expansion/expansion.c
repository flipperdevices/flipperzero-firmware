#include "expansion.h"

#include <furi_hal_power.h>
#include <furi_hal_serial.h>
#include <furi_hal_serial_control.h>

#include <furi.h>

#include <rpc/rpc.h>

#include "expansion_settings.h"
#include "expansion_protocol.h"

#define TAG "ExpansionWorker"

#define EXPANSION_INACTIVE_TIMEOUT_MS (250U)
#define EXPANSION_BUFFER_SIZE (sizeof(ExpansionFrame))

typedef enum {
    ExpansionStateDisabled,
    ExpansionStateEnabled,
    ExpansionStateRunning,
} ExpansionState;

typedef enum {
    ExpansionSessionStateHandShake,
    ExpansionSessionStateConnected,
    ExpansionSessionStateRpcActive,
} ExpansionSessionState;

typedef enum {
    ExpansionSessionExitReasonUnknown,
    ExpansionSessionExitReasonUser,
    ExpansionSessionExitReasonError,
    ExpansionSessionExitReasonTimeout,
} ExpansionSessionExitReason;

typedef enum {
    ExpansionFlagStop = 1 << 0,
    ExpansionFlagData = 1 << 1,
} ExpansionFlag;

#define EXPANSION_ALL_FLAGS (ExpansionFlagData | ExpansionFlagStop)

struct Expansion {
    ExpansionState state;
    ExpansionSessionState session_state;
    ExpansionSessionExitReason exit_reason;
    ExpansionFrame rx_frame;
    FuriStreamBuffer* rx_buf;
    FuriSemaphore* tx_semaphore;
    FuriMutex* state_mutex;
    FuriThread* worker_thread;
    FuriHalSerialId serial_id;
    FuriHalSerialHandle* serial_handle;
    RpcSession* rpc_session;
};

static void expansion_detect_callback(void* context);

// Called in UART IRQ context
static void expansion_serial_rx_callback(uint8_t data, void* context) {
    furi_assert(context);
    Expansion* instance = context;

    furi_stream_buffer_send(instance->rx_buf, &data, sizeof(data), 0);
    furi_thread_flags_set(furi_thread_get_id(instance->worker_thread), ExpansionFlagData);
}

static size_t expansion_receive_callback(uint8_t* data, size_t data_size, void* context) {
    Expansion* instance = context;

    size_t received_size = 0;

    while(true) {
        received_size += furi_stream_buffer_receive(
            instance->rx_buf, data + received_size, data_size - received_size, 0);

        if(received_size == data_size) break;

        const uint32_t flags = furi_thread_flags_wait(
            EXPANSION_ALL_FLAGS, FuriFlagWaitAny, EXPANSION_INACTIVE_TIMEOUT_MS);

        if(flags == FuriFlagErrorTimeout) {
            //  Did not receive any flags, exiting due to timeout
            instance->exit_reason = ExpansionSessionExitReasonTimeout;
            break;
        } else if(flags & FuriFlagError) {
            //  Error occurred, exiting due to error
            instance->exit_reason = ExpansionSessionExitReasonError;
            break;
        } else if(flags & ExpansionFlagStop) {
            // Exiting due to explicit request
            instance->exit_reason = ExpansionSessionExitReasonUser;
            break;
        } else if(flags & ExpansionFlagData) {
            // Go to buffer reading
            continue;
        } else {
            // Did not receive any flags, exiting due to timeout
            instance->exit_reason = ExpansionSessionExitReasonTimeout;
            break;
        }
    }

    return received_size;
}

static size_t expansion_send_callback(const uint8_t* data, size_t data_size, void* context) {
    Expansion* instance = context;
    furi_hal_serial_tx(instance->serial_handle, data, data_size);
    furi_hal_serial_tx_wait_complete(instance->serial_handle);
    return data_size;
}

static bool expansion_send_heartbeat(Expansion* instance) {
    ExpansionFrame frame = {
        .header.type = ExpansionFrameTypeHeartbeat,
        .content.heartbeat = {},
    };

    return expansion_frame_encode(&frame, expansion_send_callback, instance);
}

static bool expansion_send_status_response(Expansion* instance, ExpansionFrameError error) {
    ExpansionFrame frame = {
        .header.type = ExpansionFrameTypeStatus,
        .content.status.error = error,
    };

    return expansion_frame_encode(&frame, expansion_send_callback, instance);
}

static bool
    expansion_send_data_response(Expansion* instance, const uint8_t* data, size_t data_size) {
    furi_assert(data_size <= EXPANSION_MAX_DATA_SIZE);

    ExpansionFrame frame = {
        .header.type = ExpansionFrameTypeData,
        .content.data.size = data_size,
    };

    memcpy(frame.content.data.bytes, data, data_size);
    return expansion_frame_encode(&frame, expansion_send_callback, instance);
}

// Called in Rpc session thread context
static void expansion_rpc_send_callback(void* context, uint8_t* data, size_t data_size) {
    Expansion* instance = context;

    for(size_t sent_data_size = 0; sent_data_size < data_size;) {
        if(furi_semaphore_acquire(instance->tx_semaphore, EXPANSION_INACTIVE_TIMEOUT_MS) !=
           FuriStatusOk) {
            // TODO: this should not set ExitReason to User
            furi_thread_flags_set(furi_thread_get_id(instance->worker_thread), ExpansionFlagStop);
            break;
        }

        const size_t current_data_size = MIN(data_size - sent_data_size, EXPANSION_MAX_DATA_SIZE);
        if(!expansion_send_data_response(instance, data + sent_data_size, current_data_size))
            break;
        sent_data_size += current_data_size;
    }
}

static bool expansion_rpc_session_open(Expansion* instance) {
    Rpc* rpc = furi_record_open(RECORD_RPC);
    instance->rpc_session = rpc_session_open(rpc, RpcOwnerUnknown);

    if(instance->rpc_session) {
        instance->tx_semaphore = furi_semaphore_alloc(1, 1);
        rpc_session_set_context(instance->rpc_session, instance);
        rpc_session_set_send_bytes_callback(instance->rpc_session, expansion_rpc_send_callback);
    }

    return instance->rpc_session != NULL;
}

static void expansion_rpc_session_close(Expansion* instance) {
    if(instance->rpc_session) {
        rpc_session_close(instance->rpc_session);
        furi_semaphore_free(instance->tx_semaphore);
    }

    furi_record_close(RECORD_RPC);
}

static inline bool expansion_is_supported_baud_rate(uint32_t baud_rate) {
    // TODO: Should this be in furi_hal_serial?
    return baud_rate >= 9600UL && baud_rate <= 1000000UL;
}

static bool expansion_handle_session_state_handshake(Expansion* instance) {
    bool success = false;

    do {
        if(instance->rx_frame.header.type != ExpansionFrameTypeBaudRate) break;
        const uint32_t baud_rate = instance->rx_frame.content.baud_rate.baud;
        if(expansion_is_supported_baud_rate(baud_rate)) {
            instance->session_state = ExpansionSessionStateConnected;
            // Send response on previous baud rate
            if(!expansion_send_status_response(instance, ExpansionFrameErrorNone)) break;
            furi_hal_serial_set_br(instance->serial_handle, baud_rate);

        } else {
            if(!expansion_send_status_response(instance, ExpansionFrameErrorBaudRate)) break;
        }
        success = true;
    } while(false);

    return success;
}

static bool expansion_handle_session_state_connected(Expansion* instance) {
    bool success = false;

    do {
        if(instance->rx_frame.header.type == ExpansionFrameTypeControl) {
            if(instance->rx_frame.content.control.command != ExpansionFrameControlCommandStartRpc)
                break;
            instance->session_state = ExpansionSessionStateRpcActive;
            if(!expansion_rpc_session_open(instance)) break;
            if(!expansion_send_status_response(instance, ExpansionFrameErrorNone)) break;

        } else if(instance->rx_frame.header.type == ExpansionFrameTypeHeartbeat) {
            if(!expansion_send_heartbeat(instance)) break;

        } else {
            break;
        }
        success = true;
    } while(false);

    return success;
}

static bool expansion_handle_session_state_rpc_active(Expansion* instance) {
    bool success = false;

    do {
        if(instance->rx_frame.header.type == ExpansionFrameTypeData) {
            if(!expansion_send_status_response(instance, ExpansionFrameErrorNone)) break;

            const size_t size_consumed = rpc_session_feed(
                instance->rpc_session,
                instance->rx_frame.content.data.bytes,
                instance->rx_frame.content.data.size,
                EXPANSION_INACTIVE_TIMEOUT_MS);
            if(size_consumed != instance->rx_frame.content.data.size) break;

        } else if(instance->rx_frame.header.type == ExpansionFrameTypeControl) {
            if(instance->rx_frame.content.control.command != ExpansionFrameControlCommandStopRpc)
                break;
            instance->session_state = ExpansionSessionStateConnected;
            expansion_rpc_session_close(instance);
            if(!expansion_send_status_response(instance, ExpansionFrameErrorNone)) break;

        } else if(instance->rx_frame.header.type == ExpansionFrameTypeStatus) {
            if(instance->rx_frame.content.status.error != ExpansionFrameErrorNone) break;
            furi_semaphore_release(instance->tx_semaphore);

        } else if(instance->rx_frame.header.type == ExpansionFrameTypeHeartbeat) {
            if(!expansion_send_heartbeat(instance)) break;

        } else {
            break;
        }
        success = true;
    } while(false);

    return success;
}

static inline void expansion_state_machine(Expansion* instance) {
    static bool (*const expansion_session_state_handlers[])(Expansion*) = {
        [ExpansionSessionStateHandShake] = expansion_handle_session_state_handshake,
        [ExpansionSessionStateConnected] = expansion_handle_session_state_connected,
        [ExpansionSessionStateRpcActive] = expansion_handle_session_state_rpc_active,
    };

    while(true) {
        if(!expansion_frame_decode(&instance->rx_frame, expansion_receive_callback, instance))
            break;
        if(!expansion_session_state_handlers[instance->session_state](instance)) break;
    }
}

static int32_t expansion_worker(void* context) {
    furi_assert(context);
    Expansion* instance = context;

    furi_hal_power_insomnia_enter();

    furi_hal_serial_control_set_expansion_callback(instance->serial_id, NULL, NULL);

    instance->serial_handle = furi_hal_serial_control_acquire(instance->serial_id);
    furi_check(instance->serial_handle);

    instance->rx_buf = furi_stream_buffer_alloc(EXPANSION_BUFFER_SIZE, 1);
    instance->session_state = ExpansionSessionStateHandShake;
    instance->exit_reason = ExpansionSessionExitReasonUnknown;

    furi_hal_serial_init(instance->serial_handle, EXPANSION_DEFAULT_BAUD_RATE);
    furi_hal_serial_set_rx_callback(
        instance->serial_handle, expansion_serial_rx_callback, instance);

    if(expansion_send_heartbeat(instance)) {
        expansion_state_machine(instance);
    }

    if(instance->session_state == ExpansionSessionStateRpcActive) {
        expansion_rpc_session_close(instance);
    }

    furi_hal_serial_control_release(instance->serial_handle);
    furi_stream_buffer_free(instance->rx_buf);

    // TODO: do we really need to disable expansion callback?
    // if(instance->exit_reason == ExpansionSessionExitReasonTimeout) {
    // Thread exited due to timeout, and no disable request was issued by user code
    // Re-enable the expansion detection interrupt and be ready to establish a new connection
    furi_mutex_acquire(instance->state_mutex, FuriWaitForever);
    instance->state = ExpansionStateEnabled;
    furi_hal_serial_control_set_expansion_callback(
        instance->serial_id, expansion_detect_callback, instance);
    furi_mutex_release(instance->state_mutex);
    // }

    furi_hal_power_insomnia_exit();

    return 0;
}

// Called from the serial control thread
static void expansion_detect_callback(void* context) {
    furi_assert(context);
    Expansion* instance = context;

    furi_mutex_acquire(instance->state_mutex, FuriWaitForever);

    if(instance->state == ExpansionStateEnabled) {
        instance->state = ExpansionStateRunning;
        furi_thread_start(instance->worker_thread);
    }

    furi_mutex_release(instance->state_mutex);
}

static Expansion* expansion_alloc() {
    Expansion* instance = malloc(sizeof(Expansion));

    instance->state_mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    instance->worker_thread = furi_thread_alloc_ex(TAG, 1024, expansion_worker, instance);

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

// Public API functions

void expansion_enable(Expansion* instance, FuriHalSerialId serial_id) {
    expansion_disable(instance);

    furi_mutex_acquire(instance->state_mutex, FuriWaitForever);

    instance->serial_id = serial_id;
    instance->state = ExpansionStateEnabled;

    furi_hal_serial_control_set_expansion_callback(
        instance->serial_id, expansion_detect_callback, instance);

    furi_mutex_release(instance->state_mutex);
}

void expansion_disable(Expansion* instance) {
    furi_mutex_acquire(instance->state_mutex, FuriWaitForever);

    if(instance->state == ExpansionStateRunning) {
        furi_thread_flags_set(furi_thread_get_id(instance->worker_thread), ExpansionFlagStop);
        furi_thread_join(instance->worker_thread);
    } else if(instance->state == ExpansionStateEnabled) {
        furi_hal_serial_control_set_expansion_callback(instance->serial_id, NULL, NULL);
    }

    instance->state = ExpansionStateDisabled;

    furi_mutex_release(instance->state_mutex);
}

#include <furi.h>

#include <furi_hal_resources.h>

#include <furi_hal_serial.h>
#include <furi_hal_serial_control.h>

#include <pb.h>
#include <pb_decode.h>
#include <pb_encode.h>

#include <flipper.pb.h>

#include <expansion/expansion.h>
#include <expansion/expansion_protocol.h>

#define TAG "ExpansionTest"

#define HOST_SERIAL_ID (FuriHalSerialIdLpuart)
#define MODULE_SERIAL_ID (FuriHalSerialIdUsart)

typedef enum {
    ExpansionTestAppFlagData = 1U << 0,
    ExpansionTestAppFlagExit = 1U << 1,
} ExpansionTestAppFlag;

#define EXPANSION_TEST_APP_ALL_FLAGS (ExpansionTestAppFlagData | ExpansionTestAppFlagExit)

typedef enum {
    ExpansionTestAppStateHandShake,
    ExpansionTestAppStateStartRpc,
    ExpansionTestAppStateRpcMkDir,
    ExpansionTestAppStateRpcExit,
    ExpansionTestAppStateError,
} ExpansionTestAppState;

typedef struct {
    FuriThreadId thread_id;
    Expansion* expansion;
    FuriHalSerialHandle* handle;
    FuriStreamBuffer* buf;
    ExpansionFrame frame;
    ExpansionTestAppState state;
    PB_Main msg;
} ExpansionTestApp;

static void expansion_test_app_serial_rx_callback(uint8_t data, void* context) {
    ExpansionTestApp* app = context;
    furi_stream_buffer_send(app->buf, &data, sizeof(data), 0);
    furi_thread_flags_set(app->thread_id, ExpansionTestAppFlagData);
}

static ExpansionTestApp* expansion_test_app_alloc() {
    ExpansionTestApp* instance = malloc(sizeof(ExpansionTestApp));
    instance->buf = furi_stream_buffer_alloc(64, 1);
    return instance;
}

static void expansion_test_app_free(ExpansionTestApp* instance) {
    furi_stream_buffer_free(instance->buf);
    free(instance);
}

static void expansion_test_app_start(ExpansionTestApp* instance) {
    instance->thread_id = furi_thread_get_current_id();
    instance->expansion = furi_record_open(RECORD_EXPANSION);
    instance->handle = furi_hal_serial_control_acquire(MODULE_SERIAL_ID);
    // Configure the serial port
    furi_hal_serial_init(instance->handle, EXPANSION_DEFAULT_BAUD_RATE);
    // Start waiting for the initial pulse
    expansion_enable(instance->expansion, HOST_SERIAL_ID);

    furi_hal_serial_set_rx_callback(
        instance->handle, expansion_test_app_serial_rx_callback, instance);

    // Send pulses to emulate module insertion
    const uint8_t init = 0xAA;
    furi_hal_serial_tx(instance->handle, &init, sizeof(init));
    furi_hal_serial_tx_wait_complete(instance->handle);
}

static void expansion_test_app_stop(ExpansionTestApp* instance) {
    // Give back the module handle
    furi_hal_serial_control_release(instance->handle);
    // Turn expansion module support off
    expansion_disable(instance->expansion);
    furi_record_close(RECORD_EXPANSION);
}

static size_t expansion_test_app_receive_callback(uint8_t* data, size_t data_size, void* context) {
    ExpansionTestApp* instance = context;

    size_t received_size = 0;

    while(true) {
        received_size += furi_stream_buffer_receive(
            instance->buf, data + received_size, data_size - received_size, 0);
        if(received_size == data_size) break;

        const uint32_t flags =
            furi_thread_flags_wait(EXPANSION_TEST_APP_ALL_FLAGS, FuriFlagWaitAny, FuriWaitForever);
        // TODO: Some way to exit out of this function on request
        UNUSED(flags);
    }

    return received_size;
}

static size_t
    expansion_test_app_send_callback(const uint8_t* data, size_t data_size, void* context) {
    ExpansionTestApp* instance = context;

    furi_hal_serial_tx(instance->handle, data, data_size);

    return data_size;
}

static void
    expansion_test_app_send_baud_rate_request(ExpansionTestApp* instance, uint32_t baud_rate) {
    ExpansionFrame frame = {
        .header.type = ExpansionFrameTypeBaudRate,
        .content.baud_rate.baud = baud_rate,
    };
    expansion_frame_encode(&frame, expansion_test_app_send_callback, instance);
}

static void expansion_test_app_send_control_request(
    ExpansionTestApp* instance,
    ExpansionFrameControlCommand command) {
    ExpansionFrame frame = {
        .header.type = ExpansionFrameTypeControl,
        .content.control.command = command,
    };
    expansion_frame_encode(&frame, expansion_test_app_send_callback, instance);
}

// static void expansion_test_app_send_data_request(
//     ExpansionTestApp* instance,
//     const uint8_t* data,
//     size_t data_size) {
//     furi_assert(data_size <= EXPANSION_MAX_DATA_SIZE);
//
//     ExpansionFrame frame = {
//         .header.type = ExpansionFrameTypeData,
//         .content.data.size = data_size,
//     };
//
//     memcpy(frame.content.data.bytes, data, data_size);
//     expansion_frame_encode(&frame, expansion_test_app_send_callback, instance);
// }

static void expansion_test_app_send_rpc_request(ExpansionTestApp* instance, PB_Main* message) {
    ExpansionFrame frame;
    frame.header.type = ExpansionFrameTypeData;

    pb_ostream_t stream =
        pb_ostream_from_buffer(frame.content.data.bytes, sizeof(frame.content.data.bytes));
    pb_encode_ex(&stream, &PB_Main_msg, message, PB_ENCODE_DELIMITED);
    pb_release(&PB_Main_msg, message);

    frame.content.data.size = stream.bytes_written;
    expansion_frame_encode(&frame, expansion_test_app_send_callback, instance);
}

static void expansion_test_app_send_request(ExpansionTestApp* instance) {
    switch(instance->state) {
    case ExpansionTestAppStateHandShake:
        expansion_test_app_send_baud_rate_request(instance, 230400);
        break;
    case ExpansionTestAppStateStartRpc:
        expansion_test_app_send_control_request(instance, ExpansionFrameControlCommandStartRpc);
        break;
    case ExpansionTestAppStateRpcMkDir:
        instance->msg.command_id = 1;
        instance->msg.command_status = PB_CommandStatus_OK;
        instance->msg.which_content = PB_Main_storage_mkdir_request_tag;
        instance->msg.has_next = false;
        instance->msg.content.storage_mkdir_request.path = "/ext/External_rpc_test";

        expansion_test_app_send_rpc_request(instance, &instance->msg);
        break;
    case ExpansionTestAppStateRpcExit:
        furi_delay_ms(100);
        // expansion_test_app_send_control_request(instance, ExpansionFrameControlCommandStopRpc);
        break;
    default:
        furi_crash();
    }
}

static void expansion_test_app_process_response(ExpansionTestApp* instance) {
    // TODO: check return value
    expansion_frame_decode(&instance->frame, expansion_test_app_receive_callback, instance);

    const ExpansionFrameType frame_type = instance->frame.header.type;

    switch(instance->state) {
    case ExpansionTestAppStateHandShake:
        if(frame_type == ExpansionFrameTypeStatus) {
            if(instance->frame.content.status.error == ExpansionFrameErrorNone) {
                furi_hal_serial_set_br(instance->handle, 230400);
                instance->state = ExpansionTestAppStateStartRpc;
                furi_delay_ms(100);
            }
        }
        break;
    case ExpansionTestAppStateStartRpc:
        if(frame_type == ExpansionFrameTypeStatus) {
            if(instance->frame.content.status.error == ExpansionFrameErrorNone) {
                instance->state = ExpansionTestAppStateRpcMkDir;
            }
        }
        break;
    case ExpansionTestAppStateRpcMkDir:
        if(frame_type == ExpansionFrameTypeStatus) {
            if(instance->frame.content.status.error == ExpansionFrameErrorNone) {
                // Nothing, really
                instance->state = ExpansionTestAppStateRpcExit;
            }
        }
        break;
    case ExpansionTestAppStateRpcExit:
        furi_delay_ms(100);
        break;
    default:
        furi_crash();
    }
}

int32_t expansion_test_app(void* p) {
    UNUSED(p);

    ExpansionTestApp* app = expansion_test_app_alloc();
    expansion_test_app_start(app);

    // Give the other side time to react
    furi_delay_ms(100);

    while(true) {
        expansion_test_app_send_request(app);
        expansion_test_app_process_response(app);
    }

    expansion_test_app_stop(app);
    expansion_test_app_free(app);

    return 0;
}

#include "uart.h"
#include <Arduino.h>
#include <pb.h>
#include <pb_common.h>
#include <pb_encode.h>
#include <pb_decode.h>

#include "flipper.pb.h"

#include "bitmaps.h"
#include "expansion_protocol.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/stream_buffer.h"
#include "driver/uart.h"
#include "driver/gpio.h"


static const char *TAG = "uart_events";


#ifndef MAX
#define MAX(a, b) ((a) > (b)) ? (a) : (b)
#endif

#ifndef MIN
#define MIN(a, b) ((a) < (b)) ? (a) : (b)
#endif

#define UART_INIT_BAUD_RATE (9600UL)
#define UART_BAUD_RATE (115200UL)

#define BUF_SIZE (1024)

#define UART_ID UART_NUM_0

#define EXPANSION_MODULE_TIMEOUT_MS (EXPANSION_PROTOCOL_TIMEOUT_MS - 50UL)
#define EXPANSION_MODULE_STARTUP_DELAY_MS (250UL)
#define EXPANSION_MODULE_BUFFER_SIZE (sizeof(ExpansionFrame) + sizeof(ExpansionFrameChecksum))

static StreamBufferHandle_t stream;
static PB_Main rpc_message;
bool should_run_task = true;

// RX interrupt handler
void uart_on_rx() {
    while (Serial.available()) {
        uint8_t ch = Serial.read();
        xStreamBufferSend(stream, &ch, sizeof(ch), pdMS_TO_TICKS(EXPANSION_MODULE_STARTUP_DELAY_MS));
    }
}
// Receive frames
static size_t expansion_receive_callback(uint8_t* data, size_t data_size, void* context) {
    size_t received_size = 0;
    while(true) {
        const size_t received_size_cur = xStreamBufferReceive(
            stream,
            data + received_size,
            data_size - received_size,
            EXPANSION_MODULE_TIMEOUT_MS);
        
        
        if(received_size_cur == 0) break;
        received_size += received_size_cur;
        if(received_size == data_size) break;
    }

    return received_size;
}
static inline bool expansion_receive_frame(ExpansionFrame* frame) {
    return expansion_protocol_decode(frame, expansion_receive_callback, NULL) ==
           ExpansionProtocolStatusOk;
}
static inline bool expansion_is_heartbeat_frame(const ExpansionFrame* frame) {
    return frame->header.type == ExpansionFrameTypeHeartbeat;
}
static inline bool expansion_is_data_frame(const ExpansionFrame* frame) {
    return frame->header.type == ExpansionFrameTypeData;
}
static inline bool expansion_is_success_frame(const ExpansionFrame* frame) {
    return frame->header.type == ExpansionFrameTypeStatus &&
           frame->content.status.error == ExpansionFrameErrorNone;
}
// Send frames
static size_t expansion_send_callback(const uint8_t* data, size_t data_size, void* context) {
    Serial.write( data, data_size);
    return data_size;
}
static inline bool expansion_send_frame(const ExpansionFrame* frame) {
    return expansion_protocol_encode(frame, expansion_send_callback, NULL) ==
           ExpansionProtocolStatusOk;
}
static inline bool expansion_send_heartbeat() {
    ExpansionFrame frame;
    frame.header.type = ExpansionFrameTypeHeartbeat;
    frame.content.heartbeat = {};

    return expansion_send_frame(&frame);
}
static inline bool expansion_send_status(ExpansionFrameError error) {
    ExpansionFrame frame;
    frame.header.type = ExpansionFrameTypeStatus,
    frame.content.status.error = error;

    return expansion_send_frame(&frame);
}
static bool expansion_send_baud_rate_request(uint32_t baud_rate) {
    ExpansionFrame frame;
    frame.header.type = ExpansionFrameTypeBaudRate;
    frame.content.baud_rate.baud = baud_rate;

    return expansion_send_frame(&frame);
}
static bool expansion_send_control_request(ExpansionFrameControlCommand command) {
    ExpansionFrame frame;
    frame.header.type = ExpansionFrameTypeControl;
    frame.content.control.command = command;

    return expansion_send_frame(&frame);
}
static bool expansion_send_data_request(const uint8_t* data, size_t data_size) {
    ExpansionFrame frame;
    frame.header.type = ExpansionFrameTypeData;
    frame.content.data.size = data_size;

    memcpy(frame.content.data.bytes, data, data_size);
    return expansion_send_frame(&frame);
}
// Rpc functions
typedef struct {
    ExpansionFrame frame;
    size_t read_size; // Number of bytes already read from the data frame
} ExpansionRpcContext;

static inline bool expansion_rpc_is_read_complete(const ExpansionRpcContext* ctx) {
    return ctx->frame.content.data.size == ctx->read_size;
}

// Receive a frame while maintaining idle connection
static inline bool expansion_receive_frame_rpc_mode(ExpansionFrame* frame) {
    bool heartbeat_pending = false;

    while(true) {
        const ExpansionProtocolStatus status =
            expansion_protocol_decode(frame, expansion_receive_callback, NULL);

        if(status == ExpansionProtocolStatusErrorCommunication) {
            if(!heartbeat_pending && expansion_send_heartbeat()) {
                heartbeat_pending = true;
                continue;
            } else {
                return false;
            }

        } else if(status != ExpansionProtocolStatusOk) {
            return false;
        }

        if(expansion_is_heartbeat_frame(frame)) {
            heartbeat_pending = false;
        } else {
            return true;
        }
    }
}
static bool
    expansion_rpc_decode_callback(pb_istream_t* stream, pb_byte_t* data, size_t data_size) {
    ExpansionRpcContext* ctx =(ExpansionRpcContext*)stream->state;
    size_t received_size = 0;

    while(received_size != data_size) {
        if(expansion_rpc_is_read_complete(ctx)) {
            // Read next frame
            if(!expansion_receive_frame_rpc_mode(&ctx->frame)) break;
            if(!expansion_is_data_frame(&ctx->frame)) break;

            ctx->read_size = 0;
        }

        const size_t current_size =
            MIN(data_size - received_size, ctx->frame.content.data.size - ctx->read_size);
        memcpy(data + received_size, ctx->frame.content.data.bytes + ctx->read_size, current_size);

        ctx->read_size += current_size;

        if(expansion_rpc_is_read_complete(ctx)) {
            // Confirm the frame
            if(!expansion_send_status(ExpansionFrameErrorNone)) break;
        }

        received_size += current_size;
    }

    return (received_size == data_size);
}
static bool expansion_receive_rpc_message(PB_Main* message) {
    ExpansionRpcContext ctx = {};

    pb_istream_t is = {
        .callback = expansion_rpc_decode_callback,
        .state = &ctx,
        .bytes_left = SIZE_MAX,
        .errmsg = NULL,
    };

    return pb_decode_ex(&is, &PB_Main_msg, message, PB_DECODE_DELIMITED);
}
static bool
    expansion_rpc_encode_callback(pb_ostream_t* stream, const pb_byte_t* data, size_t data_size) {
    size_t sent_size = 0;

    while(sent_size != data_size) {
        const size_t current_size = MIN(data_size - sent_size, EXPANSION_PROTOCOL_MAX_DATA_SIZE);
        if(!expansion_send_data_request(data + sent_size, current_size)) break;

        ExpansionFrame rx_frame;
        if(!expansion_receive_frame(&rx_frame)) break;
        if(!expansion_is_success_frame(&rx_frame)) break;

        sent_size += current_size;
    }

    return (sent_size == data_size);
}
static bool expansion_send_rpc_message(PB_Main* message) {
    pb_ostream_t os = {
        .callback = expansion_rpc_encode_callback,
        .state = NULL,
        .max_size = SIZE_MAX,
        .bytes_written = 0,
        .errmsg = NULL,
    };

    const bool success = pb_encode_ex(&os, &PB_Main_msg, message, PB_ENCODE_DELIMITED);
    pb_release(&PB_Main_msg, message);
    return success;
}
static inline bool expansion_is_success_rpc_response(const PB_Main* message) {
    return message->command_status == PB_CommandStatus_OK &&
           message->which_content == PB_Main_empty_tag;
}
static inline bool expansion_is_input_rpc_response(const PB_Main* message) {
    return message->command_id == 0 && message->command_status == PB_CommandStatus_OK &&
           message->which_content == PB_Main_gui_send_input_event_request_tag;
}
static inline bool expansion_is_screen_frame_rpc_response(const PB_Main* message) {
    return message->command_id == 0 && message->command_status == PB_CommandStatus_OK &&
           message->which_content == PB_Main_gui_screen_frame_tag;
}

// Main states
static bool expansion_wait_ready() {
    bool success = false;

    do {
        ExpansionFrame frame;
        if(!expansion_receive_frame(&frame)) break;
        if(!expansion_is_heartbeat_frame(&frame)) break;
        success = true;
    } while(false);

    return success;
}
static bool expansion_handshake() {
    bool success = false;

    do {
        if(!expansion_send_baud_rate_request(UART_BAUD_RATE)) break;
        ExpansionFrame frame;
        if(!expansion_receive_frame(&frame)) break;
        if(!expansion_is_success_frame(&frame)) break;
        Serial.updateBaudRate(UART_BAUD_RATE);
        vTaskDelay(pdMS_TO_TICKS(EXPANSION_PROTOCOL_BAUD_CHANGE_DT_MS));
        success = true;
    } while(false);

    return success;
}
static bool expansion_start_rpc() {
    bool success = false;

    do {
        if(!expansion_send_control_request(ExpansionFrameControlCommandStartRpc)) break;
        ExpansionFrame frame;
        if(!expansion_receive_frame(&frame)) break;
        if(!expansion_is_success_frame(&frame)) break;
        success = true;
    } while(false);

    return success;
}
static uint32_t expansion_get_next_command_id() {
    static uint32_t command_id;
    while(++command_id == 0)
        ;
    return command_id;
}
static bool expansion_start_virtual_display() {
    bool success = false;

    rpc_message.command_id = expansion_get_next_command_id();
    rpc_message.command_status = PB_CommandStatus_OK;
    rpc_message.which_content = PB_Main_gui_start_virtual_display_request_tag;
    rpc_message.has_next = false;

    PB_Gui_StartVirtualDisplayRequest* request =
        &rpc_message.content.gui_start_virtual_display_request;

    request->send_input = true;
    request->has_first_frame = true;
    request->first_frame.orientation = PB_Gui_ScreenOrientation_HORIZONTAL;
    request->first_frame.data = (pb_bytes_array_t*)malloc(PB_BYTES_ARRAY_T_ALLOCSIZE(FLIPPER_BITMAP_SIZE));
    request->first_frame.data->size = FLIPPER_BITMAP_SIZE;

    memcpy(request->first_frame.data->bytes, bitmap_malveke_splash_screen, FLIPPER_BITMAP_SIZE);

    do {
        if(!expansion_send_rpc_message(&rpc_message)) break;
        if(!expansion_receive_rpc_message(&rpc_message)) break;
        if(!expansion_is_success_rpc_response(&rpc_message)) break;

        // Show the same picture on display
        uint8_t* frame_buffer = (uint8_t*)calloc(FLIPPER_BITMAP_SIZE, sizeof(uint8_t));

        bitmap_xbm_to_screen_frame(
            frame_buffer, bitmap_malveke_splash_screen, FLIPPER_SCREEN_WIDTH, FLIPPER_SCREEN_HEIGHT);

        free(frame_buffer);
        success = true;
    } while(false);

    pb_release(&PB_Main_msg, &rpc_message);
    return success;
}
static bool expansion_wait_input() {
    bool success = false;

    while(!success) {
        if(!expansion_receive_rpc_message(&rpc_message)) break;
        if(!expansion_is_input_rpc_response(&rpc_message)) break;

        const PB_Gui_SendInputEventRequest* request =
            &rpc_message.content.gui_send_input_event_request;
        success = (request->type == PB_Gui_InputType_RELEASE);
        pb_release(&PB_Main_msg, &rpc_message);
    };

    pb_release(&PB_Main_msg, &rpc_message);
    return success;
}
static bool expansion_stop_virtual_display() {
    bool success = false;

    rpc_message.command_id = expansion_get_next_command_id();
    rpc_message.command_status = PB_CommandStatus_OK;
    rpc_message.which_content = PB_Main_gui_stop_virtual_display_request_tag;
    rpc_message.has_next = false;

    if(expansion_send_rpc_message(&rpc_message)) {
        while(true) {
            if(!expansion_receive_rpc_message(&rpc_message)) break;
            if(!expansion_is_input_rpc_response(&rpc_message)) {
                success = expansion_is_success_rpc_response(&rpc_message);
                break;
            }

            pb_release(&PB_Main_msg, &rpc_message);
        }
    }

    pb_release(&PB_Main_msg, &rpc_message);
    return success;
}

void uart_protocol_init() {
    stream = xStreamBufferCreate(EXPANSION_MODULE_BUFFER_SIZE, 1);
    // init uart 0
    // Serial.begin(UART_INIT_BAUD_RATE);
    Serial.updateBaudRate(UART_INIT_BAUD_RATE);
    // enable RX interrupt
    Serial.onReceive(uart_on_rx);

    while(should_run_task) {
        // reset baud rate to initial value
        Serial.updateBaudRate(UART_INIT_BAUD_RATE);
        // announce presence (one pulse high -> low)
        Serial.write(0xF0);
        // wait for host response
        if(!expansion_wait_ready()) continue;
        // negotiate baud rate
        if(!expansion_handshake()) continue;
        // start rpc
        if(!expansion_start_rpc()) continue;

        // start virtual display
        if(!expansion_start_virtual_display()) continue;
        // wait for button press
        if(!expansion_wait_input()) continue;
        // stop virtual display
        if(!expansion_stop_virtual_display()) {
            // Si no se pudo detener la visualización virtual, establecer should_run_task en false para salir del bucle
            should_run_task = false;
            continue;
        }

        Serial.end();
        // Si expansion_stop_virtual_display() se ejecuta correctamente, salir del bucle
        vStreamBufferDelete(stream);
        break;
    }
}

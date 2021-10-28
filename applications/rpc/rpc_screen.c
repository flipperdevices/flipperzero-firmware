#include "flipper.pb.h"
#include "rpc_i.h"
#include "screen.pb.h"
#include <gui/gui.h>
#include <gui/gui_i.h>

void rpc_system_screen_frame_callback(uint8_t* data, size_t size, void* context) {
    furi_assert(data);
    furi_assert(size == 1024);
    furi_assert(context);

    Rpc* rpc = context;

    PB_Main* frame = furi_alloc(sizeof(PB_Main));

    frame->which_content = PB_Main_screen_stream_frame_tag;
    frame->command_status = PB_CommandStatus_OK;
    frame->content.screen_stream_frame.data = furi_alloc(PB_BYTES_ARRAY_T_ALLOCSIZE(size));
    uint8_t* buffer = frame->content.screen_stream_frame.data->bytes;
    uint16_t* frame_size_msg = &frame->content.screen_stream_frame.data->size;
    *frame_size_msg = size;
    memcpy(buffer, data, size);

    rpc_send_and_release(rpc, frame);

    free(frame);
}

void rpc_system_screen_start_stream_process(const PB_Main* msg_request, void* context) {
    rpc_send_and_release_empty(context, msg_request->command_id, PB_CommandStatus_OK);

    Gui* gui = furi_record_open("gui");
    gui_set_framebuffer_callback_context(gui, context);
    gui_set_framebuffer_callback(gui, rpc_system_screen_frame_callback);
    furi_record_close("gui");
}

void rpc_system_screen_stop_stream_process(const PB_Main* msg_request, void* context) {
    rpc_send_and_release_empty(context, msg_request->command_id, PB_CommandStatus_OK);

    Gui* gui = furi_record_open("gui");
    gui_set_framebuffer_callback(gui, NULL);
    gui_set_framebuffer_callback_context(gui, NULL);
    furi_record_close("gui");
}

void* rpc_system_screen_alloc(Rpc* rpc) {
    RpcHandler rpc_handler = {
        .message_handler = NULL,
        .decode_submessage = NULL,
        .context = rpc,
    };

    rpc_handler.message_handler = rpc_system_screen_start_stream_process;
    rpc_add_handler(rpc, PB_Main_screen_start_stream_request_tag, &rpc_handler);

    rpc_handler.message_handler = rpc_system_screen_stop_stream_process;
    rpc_add_handler(rpc, PB_Main_screen_stop_stream_request_tag, &rpc_handler);

    return NULL;
}

void rpc_system_screen_free(void* ctx) {
    Gui* gui = furi_record_open("gui");
    gui_set_framebuffer_callback(gui, NULL);
    gui_set_framebuffer_callback_context(gui, NULL);
    furi_record_close("gui");
}
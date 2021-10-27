#include "flipper.pb.h"
#include "rpc_i.h"
#include "screen.pb.h"
#include <gui/gui.h>
#include <gui/gui_i.h>

void rpc_system_screen_start_stream_process(const PB_Main* msg_request, void* context) {
    rpc_send_and_release_empty(context, msg_request->command_id, PB_CommandStatus_OK);

    Gui* gui = furi_record_open("gui");
    gui_set_framebuffer_callback_context(gui, context);
    gui_set_framebuffer_callback(gui, gui_rpc_screen_stream_callback);
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

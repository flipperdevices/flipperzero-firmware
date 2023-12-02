#include "../hex_viewer.h"
#include "../helpers/hex_viewer_custom_event.h"

void hex_viewer_scene_scroll_callback(void* context) {
    HexViewer* app = (HexViewer*)context;
    view_dispatcher_send_custom_event(
        app->view_dispatcher, HexViewerCustomEventMenuPercentEntered);
}

void hex_viewer_scene_scroll_on_enter(void* context) {
    furi_assert(context);
    HexViewer* app = context;

    TextInput* text_input = app->text_input;

    text_input_set_header_text(text_input, "Scroll to percentage (0..100)");
    text_input_set_result_callback(
        text_input,
        hex_viewer_scene_scroll_callback,
        app,
        app->percent_buf,
        HEX_VIEWER_PERCENT_INPUT,
        false);

    view_dispatcher_switch_to_view(app->view_dispatcher, HexViewerSceneScroll);
}

bool hex_viewer_scene_scroll_on_event(void* context, SceneManagerEvent event) {
    HexViewer* app = (HexViewer*)context;
    bool consumed = false;

    if(event.type == SceneManagerEventTypeCustom) {
        if(event.event == HexViewerCustomEventMenuPercentEntered) {
            int ipercent = atoi(app->percent_buf);
            ipercent = MIN(ipercent, 100);
            ipercent = MAX(ipercent, 0);
            float percent = ipercent / 100.0;

            uint32_t line_count = app->model->file_size / HEX_VIEWER_BYTES_PER_LINE;
            if(app->model->file_size % HEX_VIEWER_BYTES_PER_LINE != 0) line_count += 1;
            uint32_t scrollable_lines = line_count - HEX_VIEWER_LINES_ON_SCREEN;
            uint32_t target_line = (uint32_t)(percent * scrollable_lines);

            uint32_t new_file_offset = target_line * HEX_VIEWER_BYTES_PER_LINE;
            if(app->model->file_size > new_file_offset) {
                app->model->file_offset = new_file_offset;
                if(!hex_viewer_read_file(app)) new_file_offset = new_file_offset; // TODO Do smth
            }

            scene_manager_search_and_switch_to_previous_scene(
                app->scene_manager, HexViewerViewIdStartscreen);

            consumed = true;
        }
    }
    return consumed;
}

void hex_viewer_scene_scroll_on_exit(void* context) {
    UNUSED(context);
}

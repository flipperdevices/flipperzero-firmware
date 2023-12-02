#include "../hex_viewer.h"

void hex_viewer_scene_info_on_enter(void* context) {
    furi_assert(context);
    HexViewer* app = context;

    FuriString* buffer;
    buffer = furi_string_alloc();
    furi_string_printf(
        buffer,
        "File path: %s\nFile size: %lu (0x%lX)",
        furi_string_get_cstr(app->file_path),
        app->model->file_size,
        app->model->file_size);

    DialogMessage* message = dialog_message_alloc();
    dialog_message_set_header(message, "Hex Viewer v2.0", 16, 2, AlignLeft, AlignTop);
    dialog_message_set_icon(message, &I_hex_10px, 3, 2);
    dialog_message_set_text(message, furi_string_get_cstr(buffer), 3, 16, AlignLeft, AlignTop);
    dialog_message_set_buttons(message, NULL, NULL, "Back");
    dialog_message_show(app->dialogs, message);

    furi_string_free(buffer);
    dialog_message_free(message);

    scene_manager_search_and_switch_to_previous_scene(
        app->scene_manager, HexViewerViewIdStartscreen);
}

bool hex_viewer_scene_info_on_event(void* context, SceneManagerEvent event) {
    HexViewer* app = context;
    UNUSED(app);
    UNUSED(event);
    bool consumed = true;

    return consumed;
}

void hex_viewer_scene_info_on_exit(void* context) {
    HexViewer* app = context;
    UNUSED(app);
}

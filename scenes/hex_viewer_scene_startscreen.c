#include "../hex_viewer.h"
#include "../helpers/hex_viewer_custom_event.h"
#include "../views/hex_viewer_startscreen.h"

void hex_viewer_scene_startscreen_callback(HexViewerCustomEvent event, void* context) {
    furi_assert(context);
    HexViewer* app = context;
    view_dispatcher_send_custom_event(app->view_dispatcher, event);
}

void hex_viewer_scene_startscreen_on_enter(void* context) {
    furi_assert(context);
    HexViewer* app = context;
    hex_viewer_startscreen_set_callback(app->hex_viewer_startscreen, hex_viewer_scene_startscreen_callback, app);
    view_dispatcher_switch_to_view(app->view_dispatcher, HexViewerViewIdStartscreen);
}

bool hex_viewer_scene_startscreen_on_event(void* context, SceneManagerEvent event) {
    HexViewer* app = context;
    bool consumed = false;
    
    if(event.type == SceneManagerEventTypeCustom) {
        switch(event.event) {
            case HexViewerCustomEventStartscreenLeft:
                app->model->mode = !app->model->mode;
                consumed = true;
                break;
            case HexViewerCustomEventStartscreenRight:
                // TODO Dialog
                consumed = true;
                break;
            case HexViewerCustomEventStartscreenUp:
                //furi_check(furi_mutex_acquire(hex_viewer->mutex, FuriWaitForever) == FuriStatusOk);
                if(app->model->file_offset > 0) {
                    app->model->file_offset -= HEX_VIEWER_BYTES_PER_LINE;
                    if(!hex_viewer_read_file(app)) break;
                }
                consumed = true;
                //furi_mutex_release(hex_viewer->mutex);
                break;
            case HexViewerCustomEventStartscreenDown:
            {
                //furi_check(furi_mutex_acquire(hex_viewer->mutex, FuriWaitForever) == FuriStatusOk);
                uint32_t last_byte_on_screen =
                    app->model->file_offset + app->model->file_read_bytes;

                if(app->model->file_size > last_byte_on_screen) {
                    app->model->file_offset += HEX_VIEWER_BYTES_PER_LINE;
                    if(!hex_viewer_read_file(app)) break; // TODO Do smth
                }
                consumed = true;
                //furi_mutex_release(hex_viewer->mutex);
                break;
            }
            case HexViewerCustomEventStartscreenOk:
                if (!app->model->file_size) // TODO
                    scene_manager_next_scene(app->scene_manager, HexViewerSceneScene_4);
                else scene_manager_next_scene(app->scene_manager, HexViewerSceneMenu);
                consumed = true;
                break;
            case HexViewerCustomEventStartscreenBack: // TODO DElete
                notification_message(app->notification, &sequence_reset_red);
                notification_message(app->notification, &sequence_reset_green);
                notification_message(app->notification, &sequence_reset_blue);
                if(!scene_manager_search_and_switch_to_previous_scene(
                    app->scene_manager, HexViewerSceneStartscreen)) {
                        scene_manager_stop(app->scene_manager);
                        view_dispatcher_stop(app->view_dispatcher);
                    }
                consumed = true;
                break;
        }
    }
    
    return consumed;
}

void hex_viewer_scene_startscreen_on_exit(void* context) {
    HexViewer* app = context;
    UNUSED(app);
}